package com.hepta.checkvpn;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;

public class MainActivity extends AppCompatActivity {

    static {
        System.loadLibrary("checkvpn");
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button btn = this.findViewById(R.id.btn);
//        hook_getifaddrs();
        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //                    java_getNetWorkInfo();
                Log.e("Rzx","-----------------------------------------------------------------");
                net_us();
                //                getifaddrs();
            }
        });


//        getMac();
//        getMac2();
    }
    public void java_getNetWorkInfo() throws SocketException {
        Enumeration<NetworkInterface> networkInterfaces = NetworkInterface.getNetworkInterfaces();
        if (networkInterfaces != null) {
            while (networkInterfaces.hasMoreElements()) {
                NetworkInterface networkInterface = networkInterfaces.nextElement();
                System.out.println("设备在操作系统中显示的名称: " + networkInterface.getDisplayName());
                System.out.println("网络设备在操作系统中的名称: " + networkInterface.getName());
                System.out.println("网络接口是否已经开启并正常工作: " + networkInterface.isUp());
                System.out.println("网络接口的最大传输单元(MTU): " + networkInterface.getMTU());
                System.out.println("网络接口是是否是环回接口: " + networkInterface.isLoopback());
                Enumeration<InetAddress> inetAddressesList = networkInterface.getInetAddresses();
                System.out.print("网络接口的硬件地址(MAC地址): ");
                byte[] hardwareAddress = networkInterface.getHardwareAddress();
                if (hardwareAddress != null && hardwareAddress.length > 0) {
                    System.out.println(bytesToHexMac(hardwareAddress));
                } else {
                    System.out.println(networkInterface.getHardwareAddress());   //这个是ipv6,不是mac
                }
                while (inetAddressesList.hasMoreElements()) {
                    InetAddress address = inetAddressesList.nextElement();
                    System.out.println("主机地址: " + address.getHostAddress());
                }
                System.out.println("=============分隔=============");
            }
        }
    }

    public static String bytesToHexMac(byte[] bytes) {
        StringBuilder buf = new StringBuilder(bytes.length * 2);
        for (int i = 0; i < bytes.length; i++) {
            buf.append(String.format("%02x", new Integer(bytes[i] & 0xff)));
            if (i != bytes.length - 1) {
                buf.append(":");
            }
        }
        return buf.toString();
    }



    public native  void  net_us();
    public native  void  getifaddrs();
    public native  void  getMac();
    public native  void  getMac2();
}