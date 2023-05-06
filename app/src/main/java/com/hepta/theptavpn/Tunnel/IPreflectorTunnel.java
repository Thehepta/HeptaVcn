package com.hepta.theptavpn.Tunnel;

import android.os.ParcelFileDescriptor;
import android.util.Log;

import com.hepta.theptavpn.ServerConfig;

import java.io.IOException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

public class IPreflectorTunnel extends ProxyTunnel{


    private native int NativeStartVpn(int fd, String ipaddr, int port);
    private native void NativeStopVpn();

    private ExecutorService executorService;
    private boolean isrunning;


    public IPreflectorTunnel(ServerConfig config, ParcelFileDescriptor fd){
        super(config,fd);
        executorService = Executors.newFixedThreadPool(5);
    }



    @Override
    public Boolean start() {
        Thread runnable = new Thread() {
            @Override
            public void run() {
                isrunning = true;
                NativeStartVpn(NetInterface.getFd(),config.getIpaddr(),config.getPort());
                isrunning = false;
            }
        };

        executorService.submit(runnable);
        try {
            Thread.currentThread().sleep(1000);
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
        executorService.shutdown();
        return isrunning;
    }

    @Override
    public void stop() {
        Log.e("Rzx","stop");
        NativeStopVpn();
        try {
            NetInterface.close();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        try{
            executorService.awaitTermination(1 , TimeUnit.SECONDS);
        }catch (Exception e){
            e.printStackTrace();
        }

        Log.e("Rzx","stop end");

    }
}
