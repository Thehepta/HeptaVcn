package com.hepta.theptavpn

import android.content.Intent
import android.net.VpnService
import android.os.*
import android.util.Log
import android.widget.Toast
import com.hepta.theptavpn.Tunnel.IPreflectorTunnel
import com.hepta.theptavpn.Tunnel.ProxyTunnel
import com.hepta.theptavpn.Tunnel.tun2sockTunnel
import java.net.InetAddress
import java.net.UnknownHostException


class LocalVPNService : VpnService() {


    private var proxyBinder = LocalVPNService.ProxyBinder(this@LocalVPNService)
    private var tunnel: ProxyTunnel? =null
    public var Running: Boolean?=false
    private fun setupVPN(): ParcelFileDescriptor? {
        val ipaddr = MmkvManager.getAddress();
        val mtu = MmkvManager.getMtu();
        val netmask = MmkvManager.getnetMask();
        val prefixLength = Utils.netmask2prefix(netmask)
        val builder: Builder = Builder()
        builder.setMtu(mtu)
        builder.addAddress(ipaddr, prefixLength) //第二个参数子网掩码
        try {
            val address = InetAddress.getByName(VPN_ROUTE)
            builder.addRoute(address, 0) //第二个参数子网掩码
        } catch (e: UnknownHostException) {
            throw RuntimeException(e)
        }
        val allow_type = MmkvManager.getAllowType()
        when (allow_type) {
            MmkvManager.KEY_APP_ALLWO_NONE -> {
                builder.addDisallowedApplication(packageName)
            }
            MmkvManager.KEY_APP_ADD_ALLOW -> {
                builder.addAllowedApplication("")
                val applist = MmkvManager.decodeApplicationList(MmkvManager.KEY_APP_ADD_ALLOW)
                for (appPkg in applist) {
                    builder.addAllowedApplication(appPkg)
                }
            }
            MmkvManager.KEY_APP_ADD_DIS_ALLOW -> {
                builder.addDisallowedApplication(packageName)
                val applist = MmkvManager.decodeApplicationList(MmkvManager.KEY_APP_ADD_DIS_ALLOW)
                for (appPkg in applist) {
                    builder.addDisallowedApplication(appPkg)
                }
            }
        }
        return builder.setSession(getString(R.string.app_name)).establish()

    }
    private fun getTunnelType(config: ServerConfig, parcelFileDescriptor: ParcelFileDescriptor): ProxyTunnel {

        if (config.netType == 0) {
            return IPreflectorTunnel(config, parcelFileDescriptor)
        } else {
            return tun2sockTunnel(config, parcelFileDescriptor)
        }
    }

    public fun stopVpnService(){
        tunnel?.stop()
        proxyBinder.updateRunStatus(false);
    }

    public fun startVpnService(guid:String){
        val config = MmkvManager.decodeServerConfig(guid)
        setupVPN()?.let {
            tunnel = getTunnelType(config!!, it)
            val run = tunnel?.start();
            proxyBinder.updateRunStatus(run!!);
            if(!run){
                Log.e("Rzx","start vpn failed");
            }
        }
    }

    private fun showDialog(msg: String) {
        val handlerThree = Handler(Looper.getMainLooper())
        handlerThree.post { Toast.makeText(applicationContext, msg, Toast.LENGTH_LONG).show() }
    }

    public override fun onRevoke() {
        Log.e("Rzx","onRevoke")
        stopVpnService()
        proxyBinder.updateRunStatus(false);
        super.onRevoke()

    }
    public override fun onBind(intent: Intent) : IBinder {
        //前面这几行很重要，否则无法调用 onRevoke
        val action = if (intent != null) intent.action else null
        if (action != null && action == SERVICE_INTERFACE) {
            return super.onBind(intent)!!
        }
        return proxyBinder
    }

    public class ProxyBinder(private val localVPNService: LocalVPNService) : Binder(){

        private var mainActivity : MainActivity?= null
        public fun getLocalVPNService(activity: MainActivity) : LocalVPNService{
            mainActivity = activity
            return localVPNService
        }

        public fun updateRunStatus( status:Boolean){
            mainActivity?.mainViewModel?.isRunning?.postValue(status)
            localVPNService.Running=status
        }

    }

    companion object {
        init {
            System.loadLibrary("theptavpn")
        }
        private val TAG = LocalVPNService::class.java.simpleName
        private const val VPN_ROUTE = "0.0.0.0" // Intercept everything

    }
}