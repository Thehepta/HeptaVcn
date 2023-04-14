#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <csignal>
#include "tool.h"

#define MAX_LEN 1000
#define TCP_PORT 55540
#define UDP_PORT 55550

#include <iostream>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <linux/if_tun.h>
#include <net/if.h>

using namespace std;
int str_to_number(const char* str);
int tun_fd = -1;
char buffer[1500];
char buffer2[1500];


void process(int sock_status_fd, int i);

int tun_alloc(char *dev, int flags) {

    struct ifreq ifr;
    int fd, err;
    char *clonedev = "/dev/net/tun";

    if( (fd = open(clonedev , O_RDWR)) < 0 ) {
        perror("Opening /dev/net/tun");
        return fd;
    }

    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = flags;

    if (*dev) {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }

    if( (err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0 ) {
        perror("ioctl(TUNSETIFF)");
        close(fd);
        return err;
    }

    strcpy(dev, ifr.ifr_name);

    return fd;
}


int main(int argc, char** argv){

    char if_name[IFNAMSIZ] = "tun111";

    int sock_fd, net_fd,optval = -1;

    struct sockaddr_in local,client;
    socklen_t remotelen;

    if ((tun_fd = tun_alloc(if_name, IFF_TUN | IFF_NO_PI)) < 0 ) {
        cout<<"Error connecting to tun/tap interface:"<<if_name;
        exit(1);
    }

    /* avoid EADDRINUSE error on bind() */
    if ( (sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(1);
    }
    socklen_t socklen;
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons(TCP_PORT);
    if (bind(sock_fd, (struct sockaddr*) &local, sizeof (struct sockaddr)) < 0) {
        perror("bind()");
        exit(1);
    }

    if (listen(sock_fd, 5) < 0) {
        perror("listen()");
        exit(1);
    }


    while (1){
        if ((net_fd = accept(sock_fd, (struct sockaddr*)&client, &remotelen)) < 0) {
            perror("accept()");
            exit(1);
        }
        int pid = fork();
        if (!pid) {
            process(net_fd, tun_fd);
        }
    }
}


void __on_error(evutil_socket_t sockfd, short event, void* arg) {

    cout<<"__on_error"<< event<<endl;
    exit(0);

//    struct bufferevent *partner = static_cast<bufferevent *>(ctx);

}
void udpconn_cb2(evutil_socket_t tap_fd, short event, void* arg) {
    int sockfd = (intptr_t)arg;
    ssize_t count =  read(tap_fd, buffer2, 1500);
    write(sockfd,buffer2,count);
    cout<<"udpconn_cb2服务器从客户端接收的数据len:"<<count<<endl;
}
void udpconn_cb(evutil_socket_t sockfd, short event, void* arg) {
    int tun_fd = (intptr_t)arg;
    ssize_t count =  read(sockfd, buffer, 1500);
    write(tun_fd, buffer, count);
    cout<<"udpconn_cb服务器从客户端接收的数据len:"<<count<<endl;
}

void *UDPHandle(void *arg) {
    char msg[128];
    size_t len = sizeof(struct sockaddr);
    int sockfd = (intptr_t)arg;


    struct event* ev_udp = nullptr;
    struct event* ev_tap = nullptr;
    struct event_base* evbase = event_base_new();
    ev_udp = event_new(evbase, sockfd, EV_READ |EV_PERSIST, udpconn_cb, (void *)tun_fd);
    ev_tap = event_new(evbase, tun_fd, EV_READ | EV_PERSIST, udpconn_cb2, (void *)sockfd);


    event_base_set(evbase, ev_udp);
    event_base_set(evbase, ev_tap);
    event_add(ev_udp, nullptr);
    event_add(ev_tap, nullptr);
    event_base_dispatch(evbase);
    close(sockfd);
}


int  create_udp_socket(int &ret_fd,short sin_family,int sin_addr){

    struct sockaddr_in src_addr;
    bzero(&src_addr,sizeof (sockaddr_in));
    src_addr.sin_family          = AF_INET;
    src_addr.sin_port            = htons(UDP_PORT);
    src_addr.sin_addr.s_addr     = htonl(INADDR_ANY);

    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    if(bind(sockfd, (sockaddr*)&src_addr, sizeof (src_addr))<0){
        cout<<"bind failed"<<endl;
        perror("123456");
    }
    ret_fd = sockfd;
    return src_addr.sin_port;
}

void process(int sock_status_fd, int tap_fd) {
    cout<<"start process:"<<getpid()<<endl;
    int port = UDP_PORT;
    int flag_srandom = read_int(sock_status_fd);
    write_int(sock_status_fd, flag_srandom);
    write_int(sock_status_fd,port);
    struct sockaddr_in src_addr,dst_addr;    //用于指定本地监听信息
    bzero(&src_addr,sizeof (sockaddr_in));
    src_addr.sin_family          = AF_INET;
    src_addr.sin_port            = htons(UDP_PORT);
    src_addr.sin_addr.s_addr     = htonl(INADDR_ANY);

    cout<<"port:"<<htons(src_addr.sin_port)<<endl;
    cout<<"addr:"<<inet_ntoa(src_addr.sin_addr)<<endl;


    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    if(bind(sockfd, (sockaddr*)&src_addr, sizeof (src_addr))<0){
        cout<<"bind failed"<<endl;
        perror("123456");
    }
    socklen_t socklen = sizeof(sockaddr_in);
    int val;
    int count = recvfrom(sockfd,&val, sizeof(int),0,(sockaddr*)&dst_addr,&socklen );
//    cout<<"udp start"<<endl;
    if(flag_srandom == val){
        cout<<"udp addr:"<<inet_ntoa(dst_addr.sin_addr)<<endl;
        cout<<"udp port:"<<htons(dst_addr.sin_port)<<endl;
        cout<<"udp successful"<<endl;
    }

    if( connect(sockfd, (struct sockaddr *) &dst_addr, socklen) < 0){
        perror("connect() error");
    }
//    cout<<"udp start1"<<endl;
//    write_int(sockfd,flag_srandom);
//    cout<<"fd:"<<sockfd<<endl;

    pthread_t tid_udp;
    pthread_create(&tid_udp, NULL, UDPHandle, (void *)sockfd);




    struct event* ev = nullptr;
    struct event_base* evbase = event_base_new();

    ev = event_new(evbase, sock_status_fd, EV_CLOSED, __on_error, nullptr);

    event_base_set(evbase, ev);
    event_add(ev, nullptr);
    event_base_dispatch(evbase);

    close(sock_status_fd);
}


int str_to_number(const char* str)
{
    int i,len, num = 0;
    len= strlen(str);

    for (i = 0; i < len;i++)
        num = num * 10 + str[i] - '0';

    return num;
}