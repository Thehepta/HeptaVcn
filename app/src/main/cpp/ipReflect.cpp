//
// Created by chic on 2023/4/9.
//
#include <arpa/inet.h>
#include <cstring>
#include <__threading_support>
#include <unistd.h>
#include <cstdlib>
#include "ipReflect.h"
#include "event2/event.h"
#include "event2/bufferevent.h"
#include "iostream"
#include "event2/buffer.h"


struct event_base* evbase ;
int tcp_control_sock_fd;

#define ERR_EXIT(m)         \
    do                      \
    {                       \
        perror(m);          \
        exit(EXIT_FAILURE); \
    } while (0);

#define TCP_PORT 55540



static int read_int(int fd) {
    int val;
    int len = read(fd, &val, sizeof(int));
    if (len != sizeof(int)) {
        ERR_EXIT("unable to read_int");

    }
    return val;
}

static void write_int(int fd, int val) {
    int written = write(fd, &val, sizeof(int));
    if (written != sizeof(int)) {
        ERR_EXIT("unable to write int");
    }
}




void __on_error(struct bufferevent *bev, short what, void *ctx) {
    LOGE("Tunnel __on_error");
    event_base_loopbreak(evbase);

}


void __on_tcp_error(evutil_socket_t sockfd, short event, void* arg) {
    LOGE("__on_tcp_error");
    event_base_loopbreak(evbase);
}


void __on_recv(struct bufferevent *read, void *ctx)
{
    struct bufferevent *write = static_cast<bufferevent *>(ctx);
    struct evbuffer *src, *dst;
    src = bufferevent_get_input(read);
    dst = bufferevent_get_output(write);
    evbuffer_add_buffer(dst, src);
}
int ipReflect_stop(){

    return shutdown(tcp_control_sock_fd,SHUT_RDWR);
}


int ipReflect_start(int tun_fd){
    LOGE("ipReflect_start");

    char * ip_addr = "192.168.31.38";
    struct sockaddr_in  remote;
    int flag_srandom ,port;
    int  udp_Tunnel_fd;
    struct event* ev = nullptr;
    evbase = event_base_new();
    struct bufferevent *Tun_BufEv, *UdpBufEv;
    socklen_t server_len = sizeof(struct sockaddr_in);

    if ((tcp_control_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(1);
    }


    memset(&remote, 0, sizeof(remote));
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = inet_addr(ip_addr);
    remote.sin_port = htons(TCP_PORT);

    /* connection request */
    if (connect(tcp_control_sock_fd, (struct sockaddr*) &remote, server_len) < 0) {
        perror("connect()");
        exit(1);
    }
    srand((unsigned)time(NULL));
    flag_srandom = rand();
    write_int(tcp_control_sock_fd, flag_srandom);
    if(flag_srandom == read_int(tcp_control_sock_fd)){
        LOGE("tcp successful");
    }
    port =  read_int(tcp_control_sock_fd);

    LOGE("udp port:%d",port);

    struct sockaddr_in udp_send_sock;                                //是用于指定对方(目的主机)信息
    udp_send_sock.sin_family = AF_INET;                                //设置为IPV4通信
    udp_send_sock.sin_addr.s_addr = inet_addr(ip_addr);                //设置目的ip
    udp_send_sock.sin_port = htons(port);            //设置目的端口去链接服务器
    udp_Tunnel_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (connect(udp_Tunnel_fd, (struct sockaddr *) &udp_send_sock, server_len)) {
        LOGE("connect failed\n");
    }
    write_int(udp_Tunnel_fd, flag_srandom);


    LOGE("tcp fd:%d",tcp_control_sock_fd);
    LOGE("udp fd:%d",udp_Tunnel_fd);

    UdpBufEv = bufferevent_socket_new(evbase, udp_Tunnel_fd,
                                      BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);

    Tun_BufEv = bufferevent_socket_new(evbase, tun_fd,
                                       BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);

    bufferevent_setcb(UdpBufEv, __on_recv, NULL, __on_error, Tun_BufEv);
    bufferevent_setcb(Tun_BufEv, __on_recv, NULL, __on_error, UdpBufEv);

    bufferevent_enable(UdpBufEv, EV_READ | EV_PERSIST|EV_CLOSED);
    bufferevent_enable(Tun_BufEv, EV_READ| EV_CLOSED | EV_PERSIST);

    ev = event_new(evbase, tcp_control_sock_fd, EV_CLOSED, __on_tcp_error, nullptr);
    event_base_set(evbase, ev);
    event_add(ev, nullptr);
    event_base_dispatch(evbase);
    event_base_free(evbase);
    event_free(ev);
    bufferevent_free(UdpBufEv);
    bufferevent_free(Tun_BufEv);
    close(udp_Tunnel_fd);
    close(tcp_control_sock_fd);
    LOGE("ipReflect_start thread exit");

    return 0;

}