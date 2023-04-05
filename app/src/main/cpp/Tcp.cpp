//
// Created by chic on 2023/3/4.
//
#include <stdio.h>
# include <stdlib.h>
#include <sys/endian.h>
#include <arpa/inet.h>
#include "Tcp.h"
#include "Tcb.h"
#include "utils.h"
Tcp::Tcp(ip_hdr *ipHdr) {
    this->IPr = ipHdr;
    uint8_t ihl = (IPr->verhl & 0x0F) * 4;
    TCPr = reinterpret_cast<tcp_hdr *>(reinterpret_cast<char *>(IPr) + ihl);
    tcp_length = htons(IPr->len) - ihl;
    unsigned short chksum1;
    chksum1 = htons(TCPr->tcpchksum);
    TCPr->tcpchksum = 0;
    if(chksum1 != tcp_checksum(TCPr, tcp_length, &IPr->srcipaddr, &IPr->destipaddr,IPr->protocol)){
        IPLOGE("TCP: Bad Checksum isn't equal");
        return;
    }
    TCPr->tcpchksum = htons(chksum1);
    netInfo.destipaddr = IPr->destipaddr;
    netInfo.destport = TCPr->destport;
    netInfo.srctport = TCPr->srcport;
    int key = chksum16(&netInfo, sizeof(NetInfo), 0, 1);

    Tcb *tcb = Tcb::getTCB(key);
    if (tcb == nullptr){
        initializeConnection(key);
        printf("initializeConnection");
    }else if (this->isSYN()){
//        replySynAck();
        IPLOGE("isSYN");

    }else if (this->isRST()){
        IPLOGE("isRST");

    }else if (this->isFIN()){
        IPLOGE("isFIN");

    }else if (this->isACK()){
        IPLOGE("isACK");
        processACK(tcb);
        return;

    }
//    TCPr->flags

}

void Tcp::initializeConnection(int ipAndPort) {
    std::string destinationAddress(inet_ntoa(IPr->destipaddr));

    if (this->isSYN())
    {
        int iSocketClient = socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in servaddr;
        memset(&servaddr,0,sizeof(servaddr));
        servaddr.sin_family=AF_INET;
        servaddr.sin_port=TCPr->destport;
        servaddr.sin_addr.s_addr=inet_addr(destinationAddress.c_str());
        int iRet = connect(iSocketClient, (const struct sockaddr *)&servaddr, sizeof(struct sockaddr));
        if (-1 == iRet)
        {
            IPLOGE("connect error!\n");
        } else{
            IPLOGE("connect successful!\n");
            Tcb* tcb = new Tcb(iSocketClient,TCBStatus::SYN_RECEIVED);
            Tcb::putTCB(ipAndPort,tcb);
            CreateSynAckPacketData();
        }
    } else{
        //发送RST

    }
}

void Tcp::replySynAck() {
//    std::string destinationAddress(inet_ntoa(IPr->destipaddr));
//    char destinationPort[6];
//    char sourcePort[6];
//
//    sprintf(destinationPort, "%d", TCPr->destport);
//    sprintf(sourcePort, "%d", TCPr->srcport);
//    std::string ipAndPort = destinationAddress+ ':'+destinationPort+':'+sourcePort;
//    Tcb *tcb = Tcb::getTCB(ipAndPort);
//    if (tcb == nullptr) {
//        //指定服务器的地址结构
//        int iSocketClient = socket(AF_INET, SOCK_STREAM, 0);
//
//        struct sockaddr_in servaddr;
//        memset(&servaddr,0,sizeof(servaddr));
//        servaddr.sin_family=AF_INET;
//        servaddr.sin_port=TCPr->destport;
//        servaddr.sin_addr.s_addr=inet_addr(destinationAddress.c_str());
//        int iRet = connect(iSocketClient, (const struct sockaddr *)&servaddr, sizeof(struct sockaddr));
//        if (-1 == iRet)
//        {
//            IPLOGE("connect error!\n");
//        } else{
//            IPLOGE("connect successful!\n");
////            tcb = new Tcb(iSocketClient,ipAndPort);
////            Tcb::putTCB(ipAndPort,tcb);
//            CreateSynAckPacketData();
//        }
//    }
}



bool Tcp::tcp_check(ip_hdr *pHdr, unsigned short i, in_addr *srcipaddr, in_addr *destipaddr) {

    unsigned short chksum1;
    int chksum2;
    struct pseudo_hdr p_hdr;
    chksum1 = htons(TCPr->tcpchksum);
    memcpy(&p_hdr.srcipaddr[0], srcipaddr, 4);
    memcpy(&p_hdr.destipaddr[0], destipaddr, 4);
    p_hdr.zero = 0;
    p_hdr.protocol = pHdr->protocol;
    p_hdr.len = htons(i);

    chksum2 = chksum16(&p_hdr.srcipaddr[0], sizeof(p_hdr), 0, 0);
    TCPr->tcpchksum = 0;
    chksum2 = chksum16(&TCPr->srcport, i, chksum2, 1);
    if (chksum2 != chksum1) {
        IPLOGE("TCP: Bad Checksum %04x (it should be %04x)\n",chksum1, chksum2);
        return false;											// returns if chksum failed!
    }
    TCPr->tcpchksum = htons(chksum1);
    return true;
}

int Tcp::tcp_checksum(struct tcp_hdr * tcpHdr, uint16_t len, in_addr *srcipaddr, in_addr *destipaddr,uint8_t protocol) {

    int chksum2;
    struct pseudo_hdr p_hdr;

    memcpy(&p_hdr.srcipaddr[0], srcipaddr, 4);
    memcpy(&p_hdr.destipaddr[0], destipaddr, 4);
    p_hdr.zero = 0;
    p_hdr.protocol = protocol;
    p_hdr.len = htons(len);
    IPLOGE("TCP: tcp_checksum is len = %d",len);

    chksum2 = chksum16(&p_hdr.srcipaddr[0], sizeof(p_hdr), 0, 0);
    IPLOGE("TCP: tcp_checksum is len2 = %d",len);
    chksum2 = chksum16(tcpHdr, len, chksum2, 1);

    return chksum2;
}

void Tcp::CreateSynAckPacketData() {
    int tcp_head_length = 20;
    TcpPacketData =new uint8_t[tcp_head_length];
    memset(TcpPacketData, 0, tcp_head_length);
    struct  tcp_hdr * TcpPacketData_tcp_hdr = reinterpret_cast<tcp_hdr *>(TcpPacketData);
    TcpPacketData_tcp_hdr->destport = TCPr->srcport;
    TcpPacketData_tcp_hdr->srcport = TCPr->destport;
    TcpPacketData_tcp_hdr->seq = get_ISN();
    TcpPacketData_tcp_hdr->ack=ntohl(htonl(TCPr->seq)+1);
    TcpPacketData_tcp_hdr->data_offset = (tcp_head_length / 4) << 4;
    TcpPacketData_tcp_hdr->flags |= ACK|SYN;
    TcpPacketData_tcp_hdr->window |= TCPr->window;
    TcpPacketData_tcp_hdr->urgent_ptr = 0;
    if(20 == tcp_head_length){
//        IPLOGE("tcp Head not option\n");
    }
    int chksum2= tcp_checksum(TcpPacketData_tcp_hdr, tcp_head_length, &IPr->srcipaddr, &IPr->destipaddr, IPr->protocol);
    TcpPacketData_tcp_hdr->tcpchksum = htons(chksum2);

}
uint8_t *Tcp::retTcpPacket() {
    return TcpPacketData;
}

int Tcp::get_ISN() {
    return ((rand()<<16)+rand());
}

void Tcp::processACK(Tcb *tcb) {
    if (tcb->getStatus() == SYN_RECEIVED) {

    }
    switch(tcb->getStatus()) {
        case SYN_RECEIVED:
            tcb->setStatus(ESTABLISHED);
            return;
        case ESTABLISHED:
            forward(tcb);
        default:
            return;
    }
}

void Tcp::forward(Tcb *pTcb) {

    int server_socket = pTcb->getSocket();
    int data_off =  (TCPr->data_offset >> 4)  * 4;

    uint8_t * data = reinterpret_cast<uint8_t *>(TCPr) +data_off;
    write(server_socket,data,tcp_length-data_off);
    uint8_t *rec_data = new uint8_t[1500];
    int ret = read(server_socket,rec_data,1500);
//    CreateRetPacketData();
    int ret_tcp_length = 20+ret;

}

uint8_t *Tcp::getTcpData() {

    return nullptr;
}
