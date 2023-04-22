package com.hepta.theptavpn.Tunnel;

import com.hepta.theptavpn.ServerConfig;

import engine.Key;

public class tun2sockTunnel extends ProxyTunnel {


    public tun2sockTunnel(ServerConfig config, int fd) {
        super(config, fd);
        String proxy = "";

        switch (config.getNetType()){
            case 1:
                proxy = "socks5://"+config.getIpaddr()+":"+config.getPort();
                break;
            case 2:
                proxy = "http://"+config.getIpaddr()+":"+config.getPort();
                break;
            default:
                break;
        }

        Key key = new Key();
        key.setMark(0);
        key.setMTU(0);
        key.setDevice("fd://" + fd); // <--- here
        key.setInterface("");
        key.setLogLevel("debug");
        key.setProxy(proxy); // <--- and here
        key.setRestAPI("");
        key.setTCPSendBufferSize("");
        key.setTCPReceiveBufferSize("");
        key.setTCPModerateReceiveBuffer(false);
        engine.Engine.insert(key);
    }

    @Override
    public void start() {
        engine.Engine.start();
    }

    @Override
    public void stop() {
        engine.Engine.stop();
    }



}
