package com.hepta.theptavpn.vpnfirewall;

import android.os.ParcelFileDescriptor;
import android.system.ErrnoException;
import android.system.Os;
import android.util.Log;

import com.hepta.theptavpn.ByteBufferPool;

import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.Inet4Address;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;

public class PingReflector extends Thread {
    private static final String TAG = "PingReflector";

    private static final int PROTOCOL_ICMP = 0x01;

    private ParcelFileDescriptor mFd;

    FileChannel vpnInput;
    FileChannel vpnOutput;
    public PingReflector(ParcelFileDescriptor fd, int mtu) {
        super("PingReflector");
        mFd = fd;
        vpnInput = new FileInputStream(fd.getFileDescriptor()).getChannel();
        vpnOutput = new FileOutputStream(fd.getFileDescriptor()).getChannel();
    }

    public void run() {
        Log.i(TAG, "PingReflector starting fd=" + mFd + " valid=" + mFd.getFileDescriptor().valid());

        ByteBuffer bufferToNetwork  = ByteBufferPool.acquire();;
        try {
            while (!interrupted() && mFd.getFileDescriptor().valid()) {

                int readBytes = vpnInput.read(bufferToNetwork);
                if (readBytes > 0){
                    bufferToNetwork.flip();

                    int version = bufferToNetwork.array()[0] >> 4;
                    if (version != 4) {
                        Log.e(TAG, "Received packet version: " + version + ". Ignoring.");
                        continue;
                    }
                    try {
                        processPacket(bufferToNetwork, version, readBytes, 0);
                    } catch (IOException e) {
                        Log.w(TAG, "Failed processing packet", e);
                    }
                }
                bufferToNetwork.clear();
            }
        } catch (IOException e) {
            Log.e(TAG, "Error read packet: " + e.getMessage(), e);
        }

//        Log.i(TAG, "PingReflector exiting fd=" + mFd + " valid=" + mFd.getFileDescriptor().valid());
    }

    private void processPacket(ByteBuffer bufferToNetwork, int version, int len, int hdrLen) throws IOException {
        IcmpMessage echo = null;
        IcmpMessage response = null;

        DataInputStream stream = new DataInputStream(new ByteArrayInputStream(bufferToNetwork.array()));
        Ipv4Packet packet = new Ipv4Packet(stream);
        Log.i(TAG, "Packet contents:\n" + packet);

        if (packet.protocol != PROTOCOL_ICMP) {
            Log.i(TAG, "Protocol is " + packet.protocol + " not ICMP. Ignoring.");
            return;
        }

        echo = new IcmpMessage(
                new DataInputStream(new ByteArrayInputStream(packet.data)), packet.data.length);
        Log.i(TAG, "Ping packet:\n" + echo);

        // Swap src and dst IP addresses to route the packet back into the device.
        Inet4Address tmp = packet.sourceAddress;
        packet.sourceAddress = packet.destinationAddress;
        packet.destinationAddress = tmp;

        packet.setData(echo.getEncoded());
        writePacket(packet.getEncoded());
        Log.i(TAG, "Wrote packet back");
    }

    private void writePacket(byte[] buf) {
        try {
            Os.write(mFd.getFileDescriptor(), buf, 0, buf.length);
        } catch (ErrnoException | IOException e) {
            Log.e(TAG, "Error writing packet: " + e.getMessage(), e);
        }

    }

}

