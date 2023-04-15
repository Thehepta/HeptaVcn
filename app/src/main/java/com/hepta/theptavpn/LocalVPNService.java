package com.hepta.theptavpn;


import android.app.PendingIntent;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.VpnService;
import android.os.Handler;
import android.os.Looper;
import android.os.ParcelFileDescriptor;
import android.util.Log;
import android.widget.Toast;

import com.hepta.theptavpn.vpnfirewall.PingReflector;

import java.io.IOException;
import java.net.InetAddress;
import java.net.UnknownHostException;


public class LocalVPNService extends VpnService
{


    static {
        System.loadLibrary("theptavpn");
    }
    public native boolean connect_server(String serverAddress, String serverPort);

    public native void StartVpn(int fd, int prorxType);
    public native void stopVpn();

    public native void startProxyServer();
    public static final String ACTION_DISCONNECT = "ACTION_DISCONNECT";
    private static int MTU = 1400;

    private PingReflector mPingReflector = null;
    private static final String TAG = LocalVPNService.class.getSimpleName();
//    private static final String VPN_ADDRESS = "192.168.0.101"; // Only IPv4 support for now 掩码 24
    private static final String VPN_ADDRESS = "10.0.0.2"; // Only IPv4 support for now

    //    private static final String VPN_ROUTE = "192.168.0.0"; // Intercept everything  掩码 24
    private static final String VPN_ROUTE = "0.0.0.0"; // Intercept everything
    private ParcelFileDescriptor vpnInterface = null;

    private PendingIntent pendingIntent;

    @Override
    public void onCreate()
    {
        super.onCreate();
    }

    private void setupVPN()
    {
        if (vpnInterface == null)
        {
            Builder builder = new Builder();
            builder.addAddress(VPN_ADDRESS, 30); //第二个参数子网掩码
            try {
                InetAddress address = InetAddress.getByName(VPN_ROUTE);
                builder.addRoute(address, 0); //第二个参数子网掩码

            } catch (UnknownHostException e) {
                throw new RuntimeException(e);
            }
            builder.setMtu(MTU);
            //                builder.addDisallowedApplication("com.android.chrome");  //禁止这个应用通过vpn访问网络，但是不禁止网络，就像vpn不存在一样，正常访问网络,可以设置多个
            try {
                builder.addAllowedApplication("com.hepta.vpntest");  // 只允许这个应用通过vpn访问网络，其他应用不禁止网络，就像vpn不存在一样，正常访问网络，可以设置多个
            } catch (PackageManager.NameNotFoundException e) {
                throw new RuntimeException(e);
            }
//            builder.addAllowedApplication("com.tencent.mm");  //
//            builder.allowBypass();
            vpnInterface = builder.setSession(getString(R.string.app_name)).setConfigureIntent(pendingIntent).establish();
            Log.e(TAG,"fd:"+vpnInterface.getFd());
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if (intent != null && ACTION_DISCONNECT.equals(intent.getAction())) {
            disconnect();
            stopVpn();
            return START_NOT_STICKY;
        } else {
            String ServerAddress = intent.getStringExtra("serverAddress");
            String ServerPort = intent.getStringExtra("serverPort");
            setupVPN();
            StartVpn(vpnInterface.getFd(),1);
            return START_STICKY;
        }
    }

    @Override
    public boolean stopService(Intent name) {
        return super.stopService(name);
    }

    private void disconnect() {
        cleanup();
        Log.i(TAG, "Stopped");
    }

    private void connect(String serverAddress, String serverPort) {

        if(!connect_server(serverAddress,serverPort)){
            showDialog("不能连接到服务");
            return;
        };


    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();

    }

    private void cleanup()
    {
        if (mPingReflector != null) {
            mPingReflector.interrupt();
            mPingReflector = null;
        }
        try {
            if (vpnInterface != null) {
                Log.i(TAG, "Closing filedescriptor");
                vpnInterface.getFd();
                vpnInterface.close();
            }
        } catch(IOException e) {
            Log.w(TAG, "Closing filedescriptor failed", e);
        } finally {
            vpnInterface = null;
        }
    }

    private void showDialog(String msg){
        Handler handlerThree=new Handler(Looper.getMainLooper());
        handlerThree.post(new Runnable(){
            public void run(){
                Toast.makeText(getApplicationContext() ,msg,Toast.LENGTH_LONG).show();
            }
        });
    }
}
