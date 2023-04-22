package com.hepta.theptavpn.Tunnel;

import android.util.Log;

import com.hepta.theptavpn.ServerConfig;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

import engine.Key;

public class IPreflectorTunnel extends ProxyTunnel{


    private native void NativeStartVpn(int fd, String ipaddr, int port);
    private native void NativeStopVpn();

    private ExecutorService executorService;
    private Thread runnable;


    public IPreflectorTunnel(ServerConfig config, int fd){
        super(config,fd);
        executorService = Executors.newFixedThreadPool(5);
    }



    @Override
    public void start() {
        runnable = new Thread() {
            @Override
            public void run() {
//                while (!Thread.interrupted()) {
//                    try {
//                        Log.e("Rzx","线程二执行");
//                        Thread.sleep(1000);
//                    } catch (Exception e) {
//                        System.out.println(e.getMessage());
//                    }
//                }
                NativeStartVpn(NetInterface,config.getIpaddr(),config.getPort());
            }
        };

        executorService.submit(runnable);
        executorService.shutdown();
    }

    @Override
    public void stop() {
        Log.e("Rzx","stop");
//        runnable.interrupt();
        NativeStopVpn();
        try{
            executorService.awaitTermination(30 , TimeUnit.SECONDS);
        }catch (Exception e){
            e.printStackTrace();
        }

        Log.e("Rzx","stop end");

    }



}
