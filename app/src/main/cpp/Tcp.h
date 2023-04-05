//
// Created by chic on 2023/3/4.
//

#ifndef THEPTAVPN_TCP_H
#define THEPTAVPN_TCP_H


#include "IP.h"
#include "Tcb.h"

#include <string>

static  int FIN = 0x01;
static  int SYN = 0x02;
static  int RST = 0x04;
static  int PSH = 0x08;
static  int ACK = 0x10;
static  int URG = 0x20;


struct  tcp_hdr {
    unsigned short  srcport;	  // Source Port
    unsigned short  destport;	  // Destination Port
    unsigned int    seq;		  // Sequence Number
    unsigned int 	ack;		  // Acknowledgement Number
    char		    data_offset;	  // Data Offset    tcp头部长度，占4个字节，但是后面6位是保留，所以直接用一个字节来使用，表示有多少个32位字（4字节）,4位最大值是15，所以头部最长60字节。
    char		    flags;		  // Flags (Control bits)
    unsigned short  window;	  // Window Size
    unsigned short  tcpchksum;	  // TCP Checksum
    unsigned short  urgent_ptr;	  // Urgent Pointer
};

struct pseudo_hdr {		      // Pseudo Header for UDP and TCP Chksum Calculations
    char           srcipaddr[4];
    char           destipaddr[4];
    char            zero;
    char           protocol;
    unsigned short len;
};

struct NetInfo{
    in_addr		    destipaddr;	      // destination IP address
    unsigned short  destport;	  // Destination Port
    unsigned short  srctport;	  // Destination Port
};


class Tcp {
public:
    Tcp(ip_hdr*data);

    bool isRST()
    {
        return (TCPr->flags & RST) == RST;
    }

    bool isSYN()
    {
        return (TCPr->flags & SYN) == SYN;
    }
    bool isFIN()
    {
        return (TCPr->flags & FIN) == FIN;
    }
    bool isPSH()
    {
        return (TCPr->flags & PSH) == PSH;
    }
    bool isACK()
    {
        return (TCPr->flags & ACK) == ACK;
    }
    bool isURG()
    {
        return (TCPr->flags & URG) == URG;
    }


    void initializeConnection(int ipAndPort);
    uint8_t* retTcpPacket();

    void replySynAck();
    int get_ISN();
    void CreateSynAckPacketData();
    bool tcp_check(ip_hdr *pHdr, unsigned short i, in_addr *pAddr, in_addr *pInAddr);
    int tcp_checksum( struct tcp_hdr * TCPr,uint16_t i, in_addr *pAddr, in_addr *pInAddr,uint8_t protocol);

    void processACK(Tcb *tcb);
private:
    tcp_hdr * TCPr = nullptr;
    ip_hdr *IPr = nullptr;
    uint8_t *TcpPacketData = nullptr;
    int status;
    NetInfo netInfo;
    int tcp_length;
    void forward(Tcb *pTcb);

    uint8_t *getTcpData();
};


#endif //THEPTAVPN_TCP_H
