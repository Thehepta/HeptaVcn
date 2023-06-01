package com.hepta.theptavpn.Tunnel;

import android.os.ParcelFileDescriptor;

import com.hepta.theptavpn.LocalVPNService;
import com.hepta.theptavpn.ServerConfig;

public abstract class ProxyTunnel {

    public ServerConfig config ;
    public ParcelFileDescriptor NetInterface;
    public LocalVPNService localVPNService;
    public  abstract void start();
    public abstract void stop();


    public  ProxyTunnel(ServerConfig config, ParcelFileDescriptor fd , LocalVPNService localVPNService){
        this.config = config;
        NetInterface = fd;
        this.localVPNService = localVPNService;
    }


}
