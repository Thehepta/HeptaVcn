package com.hepta.theptavpn.Tunnel;

import android.os.ParcelFileDescriptor;

import com.hepta.theptavpn.ServerConfig;

public abstract class ProxyTunnel {

    public ServerConfig config ;
    public ParcelFileDescriptor NetInterface;
    public  abstract Boolean start();
    public abstract void stop();


    public  ProxyTunnel(ServerConfig config, ParcelFileDescriptor fd){
        this.config = config;
        NetInterface = fd;
    }


}
