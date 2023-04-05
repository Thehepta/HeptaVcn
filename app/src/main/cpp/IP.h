//
// Created by chic on 2023/3/4.
//

#ifndef THEPTAVPN_IP_H
#define THEPTAVPN_IP_H
#include <unistd.h>
#include <android/log.h>
#include <bits/in_addr.h>

#if 1
#define IPLOGE(...) __android_log_print(ANDROID_LOG_ERROR,"IPHandle",__VA_ARGS__)
#else

#define IPLOGE(...)

#endif

#define IPv4	            0x40
#define IPv6	            0x60

#define ICMP_PROTOCOL	    1
#define UDP_PROTOCOL	    17
#define TCP_PROTOCOL	    6

struct ip_hdr {
    char           verhl;		      // IP version and Header Length
    char           tos;		      // type of service
    unsigned short len;		      // total length
    unsigned short id;		      // identification
    unsigned short fragment;	      // Flags and Fragment Offset
    char		 ttl;		      // time to live
    char		 protocol;	      // protocol
    unsigned short hdrchksum;           // IP header Checksum
    in_addr		 srcipaddr;        // source IP address
    in_addr		 destipaddr;	      // destination IP address
};




class IP {

public:

    IP(char *string, int i);
    uint8_t* getData();
    int getDataLength();

    uint8_t     ver;		      // IP version and Header Length
    uint8_t     ihl;
    uint8_t     tos;		      // type of service
    uint16_t    len;		      // total length
    uint16_t    id;		      // identification
    uint16_t    fragmentOff;	      // Flags and Fragment Offset
    uint8_t     ttl;		      // time to live
    uint8_t		protocol;	      // protocol
    uint16_t    hdrchksum;           // IP header Checksum
    in_addr		srcipaddr;        // source IP address
    in_addr		destipaddr;	      // destination IP address
    void tcp_process();

    void icmp_process();

    void udp_process();

    void createIPacketData(uint8_t *pHdr);

private:
    uint8_t *  IPacketData= nullptr;
    int IPacketData_Length;
    struct ip_hdr * IPr;

};


#endif //THEPTAVPN_IP_H
