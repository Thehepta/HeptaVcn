//
// Created by chic on 2023/3/4.
//

#include <sys/endian.h>
#include "Icmp.h"
#include "utils.h"

Icmp::Icmp(ip_hdr *IPr) {
    unsigned short chksum1;
    unsigned short chksum2;
    short i;
    uint8_t ihl = (IPr->verhl & 0x0F) * 4;
    unsigned short  len = htons(IPr->len) - ihl;
    ICMPr = reinterpret_cast<icmp_hdr *>(reinterpret_cast<char *>(IPr) + ihl);


    chksum1 = HTONS(ICMPr->icmpchksum);
    ICMPr->icmpchksum = 0;
    chksum2 = chksum16(&ICMPr->type, len, 0, 1);
    if (chksum2 != chksum1) {
        IPLOGE("ICMP: Bad Checksum %04x (it should be %04x)\n",chksum1, chksum2);
        return;
    }
    ICMPr->icmpchksum = HTONS(chksum1);		  // restore checksum
    switch(ICMPr->type) {
        case ICMP_Echo:				  // Echo Request (8)
            IPLOGE("ICMP: Ping Request received  \r\n");
            break;
        case ICMP_Echo_Reply:
            IPLOGE("ICMP: Ping Reply received from \r\n");
            break;
        case ICMP_Destination_Unreachable:
            switch(ICMPr->code) {
                case ICMP_Protocol_Unreachable:
                    IPLOGE("ICMP Dest.Unreachable received: Protocol Unreachable\n");
                    break;
                case ICMP_Port_Unreachable:
                    IPLOGE("ICMP Dest.Unreachable received: Port Unreachable\n");
                    break;
                default:
                    IPLOGE("ICMP Dest.Unreachable received: Unknown Code %d\n",ICMPr->code);
                    break;
            }
            break;
        default:
            IPLOGE("ICMP received: unknown type %d\n",ICMPr->type);
            break;
    }
}
