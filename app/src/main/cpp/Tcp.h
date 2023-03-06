//
// Created by chic on 2023/3/4.
//

#ifndef THEPTAVPN_TCP_H
#define THEPTAVPN_TCP_H


#include "IP.h"

static  int FIN = 0x01;
static  int SYN = 0x02;
static  int RST = 0x04;
static  int PSH = 0x08;
static  int ACK = 0x10;
static  int URG = 0x20;


struct tcp_hdr {
    unsigned short srcport;	  // Source Port
    unsigned short destport;	  // Destination Port
    unsigned int	 seq;		  // Sequence Number
    unsigned int 	 ack;		  // Acknowledgement Number
    char		 data_offset;	  // Data Offset
    char		 flags;		  // Flags (Control bits)
    unsigned short window;	  // Window Size
    unsigned short tcpchksum;	  // TCP Checksum
    unsigned short urgent_ptr;	  // Urgent Pointer
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

    struct tcp_hdr * TCPr;

    void initializeConnection();
};


#endif //THEPTAVPN_TCP_H
