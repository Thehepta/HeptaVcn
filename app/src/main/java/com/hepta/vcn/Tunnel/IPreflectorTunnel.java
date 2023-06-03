package com.hepta.vcn.Tunnel;

import android.os.ParcelFileDescriptor;

import com.hepta.vcn.LocalVPNService;
import com.hepta.vcn.ServerConfig;

import java.io.IOException;

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
