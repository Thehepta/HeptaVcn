//
// Created by chic on 2023/3/4.
//

#ifndef THEPTAVPN_ICMP_H
#define THEPTAVPN_ICMP_H


#include "IP.h"



#define ICMP_Echo       		  8
#define ICMP_Echo_Reply 		  0
#define ICMP_Destination_Unreachable	  3	// Type = 3
#define ICMP_Protocol_Unreachable	  2	// Code = 2
#define ICMP_Port_Unreachable		  3	// Code = 3



struct icmp_hdr {
    char		  type;			// type of icmp message
    char 		  code;			// "subtype" of icmp message
    unsigned short  icmpchksum;	        // icmp message checksum
};




class Icmp {

public:
    Icmp(ip_hdr *string);

private:
    struct icmp_hdr * ICMPr = nullptr;

};


#endif //THEPTAVPN_ICMP_H
