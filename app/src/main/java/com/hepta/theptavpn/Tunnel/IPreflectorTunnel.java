package com.hepta.theptavpn.Tunnel;

import android.os.ParcelFileDescriptor;
import android.util.Log;

import com.hepta.theptavpn.LocalVPNService;
import com.hepta.theptavpn.ServerConfig;

import java.io.IOException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

public class IPreflectorTunnel extends ProxyTunnel{


    private native int NativeStartVpn(int fd, String ipaddr, int port);
    private native void NativeStopVpn();



    public IPreflectorTunnel(ServerConfig config, ParcelFileDescriptor fd, LocalVPNService localVPNService){
        super(config,fd,localVPNService);
    }



    @Override
    public void start() {
        Thread runnable = new Thread() {
            @Override
            public void run() {
                NativeStartVpn(NetInterface.getFd(),config.getIpaddr(),config.getPort());
                localVPNService.getProxyBinder().updateRunStatus(false);
                try {
                    NetInterface.close();
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
            }
        };
        runnable.start();
    }

    @Override
    public void stop() {
        NativeStopVpn();
        try {
            NetInterface.close();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}
