package com.hepta.theptavpn.vpnfirewall;

public final class Rfc1071 {
    static int checksum(byte[] data, int length) {
        int sum = 0;

        // High bytes (even indices)
        for (int i = 0; i < length; i += 2) {
            sum += (data[i] & 0xFF) << 8;
            sum = (sum & 0xFFFF) + (sum >> 16);
        }

        // Low bytes (odd indices)
        for (int i = 1; i < length; i += 2) {
            sum += (data[i] & 0xFF);
            sum = (sum & 0xFFFF) + (sum >> 16);
        }

        // Fix any one's-complement errors- sometimes it is necessary to rotate twice.
        sum = (sum & 0xFFFF) + (sum >> 16);
        return sum ^ 0xFFFF;
    }
}
