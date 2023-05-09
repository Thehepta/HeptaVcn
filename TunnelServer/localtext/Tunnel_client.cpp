#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <string.h>
#include <stdio.h>
#include <netinet/udp.h>


#define DATA "hello world!"     // enter data to be sent

unsigned short csum(unsigned short *buf, int nwords)
{
    unsigned long sum;

    for (sum = 0; nwords > 0; nwords--) {
        sum += *buf++;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)~sum;
}


int main()
{
    //套接字
    int sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP) );
    if (sockfd == -1)
    {
        printf("error at socket().\n");
        return 0;
    }

    //地址
    sockaddr_ll addr_ll;
    memset(&addr_ll, 0, sizeof(sockaddr_ll) );
    addr_ll.sll_family = PF_PACKET;

    ifreq ifr;
    strcpy(ifr.ifr_name, "enp89s0");
    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) == -1)
    {
        printf("error ioctl SIOCGIFINDEX\n"); return 0;
    }
    addr_ll.sll_ifindex = ifr.ifr_ifindex; //接口索引

    if (ioctl(sockfd, SIOCGIFADDR, &ifr) == -1)
    {
        printf("error ioctl SIOCGIFADDR\n"); return 0;
    }
    char* ipSrc = inet_ntoa(((struct sockaddr_in*)(&(ifr.ifr_addr)))->sin_addr);
    printf("ip address : %s\n", ipSrc); //source ip

    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) == -1)
    {
        printf("error ioctl SIOCGIFHWADDR\n"); return 0;
    }
//    unsigned char macDst[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    unsigned char macSrc[ETH_ALEN];
    memcpy(macSrc, ifr.ifr_hwaddr.sa_data, ETH_ALEN); //mac address
    printf("mac address");
    for (int i = 0; i < ETH_ALEN; i++)
        printf(":%02x", macSrc[i]);
    printf("\n");

    //填充以太网头部
    ethhdr ethheader;
    memcpy(ethheader.h_source, macSrc, ETH_ALEN);
    memcpy(ethheader.h_dest, macSrc, ETH_ALEN);
    ethheader.h_proto = htons(ETHERTYPE_IP);

    //填充IP头部
    iphdr ipheader;
    ipheader.version = 0x4;
    ipheader.ihl = 0x5;
    ipheader.tos = 0x00;
    ipheader.tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + strlen(DATA));
    ipheader.id = 0x1000;
    ipheader.frag_off = 0x0000;
    ipheader.ttl = 128;
    ipheader.protocol = 0x01;
    ipheader.check = 0;
    ipheader.saddr = inet_addr(ipSrc);
    ipheader.daddr = inet_addr(ipSrc);

    unsigned int checkSum = 0;
    unsigned int num;
    unsigned char* p = (unsigned char*)&ipheader;
    int i;
    for (i = 0; i <= 18; i += 2)
    {
        num = (p[i] << 8) + p[i + 1];
        checkSum += num;
        checkSum = (checkSum & 0xffff) + (checkSum >> 16);
    }
    checkSum = (~checkSum) & 0xffff;
    ipheader.check = htons((unsigned short)checkSum);

    //填充UDP头部
    int data_len = strlen(DATA);
    struct udphdr udp_header;
    udp_header.source = htons(8000);
    udp_header.dest = htons(55540);
    udp_header.len = htons(sizeof(struct udphdr) + data_len);
    udp_header.check = 0;
    udp_header.check = csum((unsigned short *)&udp_header, sizeof(struct udphdr) + data_len/2);


    //发送
    unsigned char sendBuf[sizeof(ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr) + data_len];
    memcpy(sendBuf, &ethheader, sizeof(ethhdr) );
    memcpy(sendBuf + sizeof(ethhdr), &ipheader, sizeof(iphdr) );
    memcpy(sendBuf + sizeof(ethhdr) + sizeof(iphdr), &udp_header, sizeof (struct udphdr));
    memcpy(sendBuf + sizeof(ethhdr) + sizeof(iphdr) + sizeof (struct udphdr), DATA, data_len);

    int len = sendto(sockfd, sendBuf, sizeof(sendBuf), 0, (const sockaddr*)&addr_ll, sizeof(sockaddr_ll));
    if (len > 0)
    {
        printf("send success.\n");
    }

    return 0;
}
