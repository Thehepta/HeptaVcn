//
// Created by chic on 2023/3/4.
//

#include <sys/endian.h>
#include <cstring>
#include "IP.h"
#include "Tcp.h"
#include "Udp.h"
#include "Icmp.h"
#include "utils.h"
IP::IP(char *data, int readlen) {

    unsigned short len;
    unsigned short chksum1;
    unsigned short chksum2;
    int i;
    IPr = (struct ip_hdr *)data;
    ihl = (IPr->verhl & 0x0F) * 4;

    if ((IPr->verhl & 0xF0) == IPv4) {
        IPLOGE("IP: packet version is  ipv4 and ihl=%d\r\n",ihl);
    } else if((IPr->verhl & 0xF0) == IPv6){
        IPLOGE("IP: packet version is  ipv6\r\n");

    }
    if ((IPr->verhl & 0x0F) != 0x05) {
        IPLOGE("IP: header options not supported!\r\n");
//        return;
    }
    if ((htons(IPr->fragment) & 0x1FFF) != 0x0000) {
        IPLOGE("IP: fragmented packets not supported!\r\n");
//        return;
    }
    memcpy(&srcipaddr, &IPr->srcipaddr, 4);
    memcpy(&destipaddr, &IPr->destipaddr, 4);
    chksum1 = htons(IPr->hdrchksum);
    IPr->hdrchksum = 0;
    chksum2 = chksum16(IPr, ihl, 0, 1);
    if (chksum2 != chksum1) {
        IPLOGE("IP: Bad Checksum %04x (it should be %04x)\n",chksum1, chksum2);
        return;				  // returns if chksum failed!
    }
    IPr->hdrchksum = htons(chksum1);	  // restore checksum
    len = htons(IPr->len) - ihl; 		// Length of IP Data
    switch (IPr->protocol) {
        case ICMP_PROTOCOL:
            IPLOGE("IP: fragmented packets is icmp_process\r\n");
            icmp_process();
            break;
        case UDP_PROTOCOL:
            IPLOGE("IP: fragmented packets is udp_process\r\n");
            udp_process();
            break;
        case TCP_PROTOCOL:
            IPLOGE("IP: fragmented packets is tcp_process\r\n");
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
    auto * tcp = new Tcp(this->IPr);
    uint8_t * tcp_head = tcp->retTcpPacket();
    if(nullptr !=tcp_head){
        createIPacketData(tcp_head);
    }
}

void IP::icmp_process() {
    auto * icmp = new Icmp(this->IPr);


}

void IP::udp_process() {
    auto * udp = new Udp(this->IPr);

}

uint8_t* IP::getData() {
    return IPacketData;
}

void IP::createIPacketData(uint8_t *tcp_head) {
    IPacketData_Length = sizeof(struct ip_hdr)+sizeof(struct tcp_hdr);
    IPacketData = new uint8_t[sizeof(struct ip_hdr)+sizeof(struct tcp_hdr)];
    struct  ip_hdr * IPt = reinterpret_cast<ip_hdr *>(IPacketData);
//    struct  tcp_hdr * TcpPacketData_tcp_hdr = reinterpret_cast<tcp_hdr *>(tcp_head);

    IPt->verhl=0x45;
    IPt->tos=0x00;
    IPt->len=htons(sizeof(struct tcp_hdr)+sizeof(struct ip_hdr));
//    ipid++;
    IPt->id =       IPr->id;
    IPt->fragment=  IPr->fragment;
    IPt->ttl=       IPr->ttl;
    IPt->protocol=  IPr->protocol;
    IPt->hdrchksum= 0;
    memcpy(&IPt->destipaddr, &IPr->srcipaddr, 4);
    memcpy(&IPt->srcipaddr, &IPr->destipaddr, 4);
    IPt->hdrchksum = htons(chksum16(IPt, (IPt->verhl & 0x0F) * 4, 0, 1));
    memcpy(IPacketData+sizeof(struct ip_hdr), tcp_head, sizeof(struct tcp_hdr));

}

int IP::getDataLength() {
    return IPacketData_Length;
}

