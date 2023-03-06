//
// Created by chic on 2023/3/4.
//
#include <stdio.h>
# include <stdlib.h>
#include <sys/endian.h>
#include <arpa/inet.h>
#include "Tcp.h"
#include "Tcb.h"
#include <string>
Tcp::Tcp(ip_hdr *IPr) {
    uint8_t ihl = (IPr->verhl & 0x0F) * 4;
    unsigned short  len = HTONS(IPr->len) - ihl;
    TCPr = reinterpret_cast<tcp_hdr *>(reinterpret_cast<char *>(IPr) + ihl);
    std::string destinationAddress(inet_ntoa(IPr->destipaddr));
    char destinationPort[6];
    char sourcePort[6];

    sprintf(destinationPort, "%d", TCPr->destport);
    sprintf(sourcePort, "%d", TCPr->srcport);
    std::string ipAndPort = destinationAddress+ ':'+destinationPort+':'+sourcePort;
    Tcb *tcb = Tcb::getTCB(ipAndPort);
    if (tcb == nullptr){
        initializeConnection();
        printf("initializeConnection");
    }else if (this->isSYN()){
        printf("isSYN");

    }else if (this->isRST()){
        printf("isRST");

    }else if (this->isFIN()){
        printf("isFIN");

    }else if (this->isACK()){
        printf("isACK");

    }
//    TCPr->flags

}

void Tcp::initializeConnection() {
    if (this->isSYN())
    {

    }
}
