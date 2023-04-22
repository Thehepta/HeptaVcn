package com.hepta.theptavpn

import android.app.PendingIntent
import android.content.Intent
import android.content.ServiceConnection
import android.net.VpnService
import android.os.*
import android.util.Log
import android.widget.Toast
import com.hepta.theptavpn.Tunnel.IPreflectorTunnel
import com.hepta.theptavpn.Tunnel.ProxyTunnel
import com.hepta.theptavpn.Tunnel.tun2sockTunnel
import com.tencent.mmkv.MMKV
import java.net.InetAddress
import java.net.UnknownHostException

class LocalVPNService : VpnService() {


    private val pendingIntent: PendingIntent? = null
    private var proxyBinder = LocalVPNService.ProxyBinder(this@LocalVPNService)
    private var tunnel: ProxyTunnel? =null
    private var Running:Boolean = false
    override fun onCreate() {
        super.onCreate()
    }

    private fun setupVPN(): ParcelFileDescriptor? {



        val builder: Builder = Builder()
        builder.addAddress(VPN_ADDRESS, 30) //第二个参数子网掩码
        try {
            val address = InetAddress.getByName(VPN_ROUTE)
            builder.addRoute(address, 0) //第二个参数子网掩码
        } catch (e: UnknownHostException) {
            throw RuntimeException(e)
        }
        builder.setMtu(MTU)
        val allow_type = MmkvManager.getAllowType()
        when(allow_type){
            MmkvManager.KEY_APP_ALLWO_BYPASS-> builder.allowBypass();

            MmkvManager.KEY_APP_ADD_ALLOW-> {
                val applist  = MmkvManager.decodeApplicationList()
                for (appPkg in applist){
                    builder.addAllowedApplication(appPkg)
                }
            }
            MmkvManager.KEY_APP_ADD_DIS_ALLOW-> {
                val applist  = MmkvManager.decodeApplicationList()
                for (appPkg in applist){
                    builder.addDisallowedApplication(appPkg)
                }
            }
        }
        val vpnInterface = builder.setSession(getString(R.string.app_name)).establish()
        return vpnInterface

    }

    private fun getTunnelType(config: ServerConfig, fd: Int): ProxyTunnel {

        if (config.netType == 0) {
            return IPreflectorTunnel(config, fd)
        } else {
            return tun2sockTunnel(config, fd)
        }
    }



    public fun stopVpnService(){
        tunnel?.stop()
        Running = false


    }

    public fun startVpnService(guid:String): Boolean{
        val config = MmkvManager.decodeServerConfig(guid)
        tunnel = getTunnelType(config!!, setupVPN()!!.detachFd())
        tunnel?.start()
        Running = true
        return true
    }

    private fun showDialog(msg: String) {
        val handlerThree = Handler(Looper.getMainLooper())
        handlerThree.post { Toast.makeText(applicationContext, msg, Toast.LENGTH_LONG).show() }
    }


    override fun unbindService(conn: ServiceConnection) {
        super.unbindService(conn)
    }


    public override fun onBind(intent: Intent) : IBinder {
        return proxyBinder
    }

    public class ProxyBinder(private val localVPNService: LocalVPNService) : Binder(){

        private var mainActivity : MainActivity?= null

        public fun getMovieService(activity: MainActivity) : LocalVPNService{
            mainActivity = activity
            return localVPNService
        }

        public fun updateRunStatus( status:Boolean){
            mainActivity?.mainViewModel?.isRunning?.postValue(status)
        }
    }

    companion object {
        init {
            System.loadLibrary("theptavpn")
        }

        const val ACTION_DISCONNECT = "ACTION_DISCONNECT"
        private const val MTU = 1400
        private val TAG = LocalVPNService::class.java.simpleName

        //    private static final String VPN_ADDRESS = "192.168.0.101"; // Only IPv4 support for now 掩码 24
        private const val VPN_ADDRESS = "10.0.0.2" // Only IPv4 support for now

        //    private static final String VPN_ROUTE = "192.168.0.0"; // Intercept everything  掩码 24
        private const val VPN_ROUTE = "0.0.0.0" // Intercept everything

    }
}