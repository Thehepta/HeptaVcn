//
// Created by chic on 2023/3/4.
//

#include <sys/endian.h>
#include <cstring>
#include "IP.h"
#include "Tcp.h"
#include "Udp.h"
#include "Icmp.h"

IP::IP(char *data, int readlen) {

    unsigned short len;
    unsigned short chksum1;
    unsigned short chksum2;
    int i;
    IPr = (struct ip_hdr *)data;
    if ((IPr->verhl & 0xF0) == IPv4) {
        IPLOGE("IP: packet version is  ipv4\r\n");
    } else if((IPr->verhl & 0xF0) == IPv6){
        IPLOGE("IP: packet version is  ipv6\r\n");

    }
    ihl = (IPr->verhl & 0x0F) * 4;
    if ((IPr->verhl & 0x0F) != 0x05) {
        IPLOGE("IP: header options not supported!\r\n");
//        return;
    }
    if ((HTONS(IPr->fragment) & 0x1FFF) != 0x0000) {
        IPLOGE("IP: fragmented packets not supported!\r\n");
//        return;
    }
    memcpy(&srcipaddr, &IPr->srcipaddr, 4);
    memcpy(&destipaddr, &IPr->destipaddr, 4);
    chksum1 = HTONS(IPr->hdrchksum);
    IPr->hdrchksum = 0;
//    chksum2 = chksum16(&IPr->verhl, (IPr->verhl & 0x0F) * 4, 0, 1);

    len = HTONS(IPr->len) - ihl; 		// Length of IP Data
    switch (IPr->protocol) {
        case ICMP_PROTOCOL:
            icmp_process();
            break;
        case UDP_PROTOCOL:
            udp_process();
            break;
        case TCP_PROTOCOL:
            tcp_process();
            break;
        default:				  // send Protocol Unreachable ICMP message
            IPLOGE("IP: fragmented packets not supported!\r\n");
//            i = ICMP_DATA_START;
//            // Copy original IP header + first 8 bytes of IP Data
//            memcpy(&tx_buf[i+4], &rx_buf[DATALINK_HDR_SIZE], sizeof(struct ip_hdr)+8);
//            ICMPt->type = ICMP_Destination_Unreachable;
//            ICMPt->code = ICMP_Protocol_Unreachable;
//            tx_buf[i]   = 0;			  // Unused (reserved)
//            tx_buf[i+1] = 0;			  // Unused (reserved)
//            tx_buf[i+2] = 0;			  // Unused (reserved)
//            tx_buf[i+3] = 0;                    // Unused (reserved)
//            icmp_send(IPr->srcipaddr, sizeof(struct icmp_hdr)+4+sizeof(struct ip_hdr)+8);
            break;
    }

}

void IP::tcp_process() {
    auto * tcp = new Tcp(IPr);

}

void IP::icmp_process() {
    auto * icmp = new Icmp(IPr);


}

void IP::udp_process() {
    auto * udp = new Udp(IPr);

}

char IP::getData() {
    return 0;
}

