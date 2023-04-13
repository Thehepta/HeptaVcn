//
// Created by chic on 2023/4/11.
//
#include <arpa/inet.h>
#include <string.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>


#define IP_ADDR "192.168.31.38"
#define PORT 55559
#define BUFFER_SIZE 1024
#define MSGSIZE 128

// 注意evutil_socket_t不仅仅可以接受socketfd，本质上是一个fd，即其他的文件描述符也是可以接受的。
void udpconn_cb(evutil_socket_t sockfd, short event, void* arg) {
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);
    read(sockfd, buffer, BUFFER_SIZE);
    printf("%s \n",buffer);
}

int main(int argc, char** argv) {

    char msg[MSGSIZE];

    size_t len = sizeof(struct sockaddr);

    struct sockaddr_in serv_addr ;
    serv_addr.sin_family          = AF_INET;
    serv_addr.sin_port            = htons(PORT);
    serv_addr.sin_addr.s_addr     = inet_addr(IP_ADDR);

    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    bind(sockfd, (sockaddr*)&serv_addr, len);


    while(1){
        int ret = recvfrom( sockfd, msg, MSGSIZE, 0, (struct sockaddr *)&serv_addr, (socklen_t *)&len );
        msg[ret]=0x0;
        printf("服务器从客户端接收的数据为:[%s],len=%d\n", msg,ret);
    }


//    struct event* ev = nullptr;
//    struct event_base* evbase = event_base_new();
//    ev = event_new(evbase, sockfd, EV_READ | EV_WRITE, udpconn_cb, nullptr);
//
//    event_base_set(evbase, ev);
//    event_add(ev, nullptr);
//    event_base_dispatch(evbase);
//
//    close(sockfd);
//    return 0;
}