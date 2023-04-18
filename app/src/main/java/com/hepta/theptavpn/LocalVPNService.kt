package com.hepta.theptavpn

import android.app.PendingIntent
import android.content.Intent
import android.content.ServiceConnection
import android.content.pm.PackageManager
import android.net.ProxyInfo
import android.net.VpnService
import android.os.*
import android.util.Log
import android.widget.Toast
import java.net.InetAddress
import java.net.UnknownHostException


class LocalVPNService : VpnService() {
    external fun connect_server(serverAddress: String?, serverPort: String?): Boolean
//    external fun StartVpn(fd: Int, prorxType: Int)

    external fun NativeStartVpn(fd: Int, ipaddr: String, port: Int, netType: Int)
    external fun NativeStopVpn( netType: Int)
    external fun startProxyServer()
    private var vpnInterface: ParcelFileDescriptor? = null
    private val pendingIntent: PendingIntent? = null
    private var vpnRunnable: VPNRunnable? = null
    private var proxyBinder = LocalVPNService.ProxyBinder(this@LocalVPNService)

    override fun onCreate() {
        super.onCreate()
    }

    private fun setupVPN() {
        if (vpnInterface == null) {
            val builder: Builder = Builder()
            builder.addAddress(VPN_ADDRESS, 30) //第二个参数子网掩码
            try {
                val address = InetAddress.getByName(VPN_ROUTE)
                builder.addRoute(address, 0) //第二个参数子网掩码
            } catch (e: UnknownHostException) {
                throw RuntimeException(e)
            }
            builder.setMtu(MTU)
            //                builder.addDisallowedApplication("com.android.chrome");  //禁止这个应用通过vpn访问网络，但是不禁止网络，就像vpn不存在一样，正常访问网络,可以设置多个
            builder.addAllowedApplication("com.hepta.vpntest") // 只允许这个应用通过vpn访问网络，其他应用不禁止网络，就像vpn不存在一样，正常访问网络，可以设置多个
            //            builder.addAllowedApplication("com.tencent.mm");  //
//            builder.allowBypass();
            vpnInterface = builder.setSession(getString(R.string.app_name)).establish()
            Log.e(TAG, "fd:" + vpnInterface!!.fd)
        }else{
            Log.e(TAG, "vpnInterfaceis not null fd:" + vpnInterface!!.fd)
        }
    }

    private fun startVpn(guid:String): Boolean{
        val config =  MmkvManager.decodeServerConfig(guid)

        setupVPN()
        vpnRunnable = VPNRunnable(config!!)
        vpnRunnable!!.start()
        return vpnRunnable!!.isAlive

    }

    public fun stopVpnService(){
        vpnRunnable!!.stopVpn()

    }

    public fun startVpnService(guid:String): Boolean{
        return  startVpn(guid)
    }


    override fun onDestroy() {
        super.onDestroy()
    }
    private fun showDialog(msg: String) {
        val handlerThree = Handler(Looper.getMainLooper())
        handlerThree.post { Toast.makeText(applicationContext, msg, Toast.LENGTH_LONG).show() }
    }

    inner class VPNRunnable(val config: ServerConfig) : Thread() {
        override fun run() {
            Log.w(TAG, "VPNRunnable thread start")
            vpnInterface?.let {
                NativeStartVpn(it.fd, config.ipaddr,config.port,config.netType)
            }
            Log.w(TAG, "VPNRunnable thread end")
            proxyBinder.updateRunStatus(false)
            vpnInterface!!.close()
            vpnInterface=null
        }

        fun stopVpn() {
            NativeStopVpn(0)
            Log.w(TAG, "VPNRunnable stopVpn")
        }
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