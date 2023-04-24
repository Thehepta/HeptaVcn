package com.hepta.theptavpn.Tunnel;

import com.hepta.theptavpn.ServerConfig;

public abstract class ProxyTunnel {

    public ServerConfig config ;
    public int NetInterface;
    public  abstract Boolean start();
    public abstract void stop();


    public  ProxyTunnel(ServerConfig config,int fd){
        this.config = config;
        NetInterface = fd;
    }

}
