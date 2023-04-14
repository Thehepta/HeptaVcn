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
ipReflect::ipReflect() {

}

int ipReflect::connetc() {

    return 0;
}

#define BUFFER_SIZE 3000

#define ERR_EXIT(m)         \
    do                      \
    {                       \
        perror(m);          \
        exit(EXIT_FAILURE); \
    } while (0);

#define TCP_PORT 55540
#define UDP_PORT 55550

char buffer[1500];
char buffer2[1500];


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


int server_socket;
int tun_fd;
struct event_base * ipEventBase = event_base_new();

void *readTun(void *arg) {

    char buffer[BUFFER_SIZE];

    while (1){
        int recvbytes = read(tun_fd,buffer, sizeof(buffer));
        if(recvbytes == 0) {  // 已断开连接
            LOGE("read client is disconnect.\n");
            break;
        }
        if(recvbytes < 0) {
//            LOGE("read err:%d.\n", errno)
            perror("read");
            continue;
        }
        write(server_socket, buffer, recvbytes);
    }

}

void *readNet(void *arg) {

    char buffer[BUFFER_SIZE];

    while (1){
        int recvbytes = recv(server_socket,buffer, sizeof(buffer),0);
        if(recvbytes == 0) {  // 已断开连接
            LOGE("recv client is disconnect.\n");
            break;
        }
        if(recvbytes < 0) {
//            LOGE("recv err:%d.\n", errno);
            perror("recv");
            continue;
        }
        write(tun_fd, buffer, recvbytes);
    }

}

//事件回调处理
void server_callback(struct bufferevent * RemoteBufEv, short sEvent, void * pArg)
{
    //成功连接通知事件
    if(BEV_EVENT_CONNECTED == sEvent)
    {
        server_socket = bufferevent_getfd(RemoteBufEv);
        pthread_t tid_net;
        pthread_t tid_tun;
        pthread_create(&tid_net, NULL, readNet, NULL);
        pthread_create(&tid_tun, NULL, readTun, NULL);
        pthread_join(tid_net, nullptr);
        pthread_join(tid_tun, nullptr);
    }
    LOGE("event_callback");
    return ;
}
void __on_error(evutil_socket_t sockfd, short event, void* arg) {

    LOGE("__on_error %d\n", event);
    pthread_exit(0);
//    struct bufferevent *partner = static_cast<bufferevent *>(ctx);

}

void udpconn_cb(evutil_socket_t read_fd, short event, void* write_fd) {

    int sockfd = (intptr_t)write_fd;
    ssize_t count =  read(read_fd, buffer, 1500);
    write(sockfd, buffer, count);
    LOGE("TUN send len:%d\n",count);
}


void *UDPHandle(void *arg) {

    int sockfd = (intptr_t)arg;

    std::string str="hekko world";
    char buff[1024];
//    while (1){
//        LOGE("connect send\n");
//        sleep(1);
//        send(sockfd, str.c_str(), str.length(), 0);
////        sendto(socket_fd, str.c_str(), str.length(), 0, (struct sockaddr*)&remote, server_len);
//    }

    struct event* ev_udp = nullptr;
    struct event* ev_tap = nullptr;
    struct event_base* evbase = event_base_new();
    ev_tap = event_new(evbase, tun_fd, EV_READ |EV_PERSIST, udpconn_cb, (void *)sockfd);
    ev_udp = event_new(evbase, sockfd, EV_READ |EV_PERSIST, udpconn_cb, (void *)tun_fd);

    event_base_set(evbase, ev_tap);
    event_base_set(evbase, ev_udp);
    event_add(ev_tap, nullptr);
    event_add(ev_udp, nullptr);
    event_base_dispatch(evbase) ;
//
//    close(sockfd);
}


void ipReflect_start(int fd){
    LOGE("ipReflect_start");
    tun_fd =fd;

    char * ip_addr = "192.168.31.38";
    struct sockaddr_in  remote;
    int sock_fd, net_fd, optval = 1;
    socklen_t server_len = sizeof(struct sockaddr_in);

    if ( (sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(1);
    }

    memset(&remote, 0, sizeof(remote));
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = inet_addr(ip_addr);
    remote.sin_port = htons(TCP_PORT);

    /* connection request */
    if (connect(sock_fd, (struct sockaddr*) &remote, server_len) < 0) {
        perror("connect()");
        exit(1);
    }
    srand((unsigned)time(NULL));
    int flag_srandom = rand();
    write_int(sock_fd, flag_srandom);
    if(flag_srandom == read_int(sock_fd)){
        LOGE("tcp successful");
    }
    int port =  read_int(sock_fd);

    LOGE("udp port:%d",port);

    struct sockaddr_in udp_send_sock;                                //是用于指定对方(目的主机)信息
    udp_send_sock.sin_family = AF_INET;                                //设置为IPV4通信
    udp_send_sock.sin_addr.s_addr = inet_addr(ip_addr);                //设置目的ip
    udp_send_sock.sin_port = htons(port);            //设置目的端口去链接服务器
    int udp_send_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (connect(udp_send_fd, (struct sockaddr *) &udp_send_sock, server_len)) {
        LOGE("connect failed\n");
    }
    write_int(udp_send_fd,flag_srandom);
//    if(flag_srandom == read_int(sock_fd)){
//        LOGE("udp successful");
//    }
    pthread_t tid_udp;
    pthread_create(&tid_udp, NULL, UDPHandle, (void*)udp_send_fd);


    struct event* ev = nullptr;
    struct event_base* evbase = event_base_new();

    ev = event_new(evbase, sock_fd, EV_CLOSED, __on_error, nullptr);

    event_base_set(evbase, ev);
    event_add(ev, nullptr);
    event_base_dispatch(evbase);

    close(sock_fd);

}

 #define MAXLINE 4096
 #define UDPPORT 8001
 #define SERVERIP "192.168.255.129"
int main1(){
    int serverfd;
    unsigned int server_addr_length, client_addr_length;
    char recvline[MAXLINE];
    char sendline[MAXLINE];
    struct sockaddr_in serveraddr , clientaddr;

    // 使用函数socket()，生成套接字文件描述符；
    if( (serverfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){
        perror("socket() error");
    }

    // 通过struct sockaddr_in 结构设置服务器地址和监听端口；
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(UDPPORT);
    server_addr_length = sizeof(serveraddr);

    // 使用bind() 函数绑定监听端口，将套接字文件描述符和地址类型变量（struct sockaddr_in ）进行绑定；
    if( bind(serverfd, (struct sockaddr *) &serveraddr, server_addr_length) < 0){
        perror("bind() error");
    }

    // 接收客户端的数据，使用recvfrom() 函数接收客户端的网络数据；
    client_addr_length = sizeof(sockaddr_in);
    int recv_length = 0;
//    recv_length = recvfrom(serverfd, recvline, sizeof(recvline), 0, (struct sockaddr *) &clientaddr, &client_addr_length);

    // 向客户端发送数据，使用sendto() 函数向服务器主机发送数据；
    int send_length = 0;
    sprintf(sendline, "hello client !");
    send_length = sendto(serverfd, sendline, sizeof(sendline), 0, (struct sockaddr *) &clientaddr, client_addr_length);
    if( send_length < 0){
        perror("sendto() error");
    }

    //关闭套接字，使用close() 函数释放资源；
    close(serverfd);

    return 0;
}