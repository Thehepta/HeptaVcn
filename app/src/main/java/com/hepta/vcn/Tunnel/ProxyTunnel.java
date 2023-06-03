package com.hepta.vcn.Tunnel;

import android.os.ParcelFileDescriptor;

import com.hepta.vcn.LocalVPNService;
import com.hepta.vcn.ServerConfig;

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
