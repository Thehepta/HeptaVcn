//
// Created by chic on 2023/2/25.
//

#include <sys/socket.h>
#include <linux/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/tcp.h>  /* for TCP_XXX defines */
#include "Sock5Client.h"
#include "event2/event.h"
#include "event2/bufferevent.h"

int Sock5Client::connect_server() {
    return 0;
}

void Sock5Client::socks5Config() {

}

void event_cb(struct bufferevent *bev, short events, void * arg)
{
    if(events & BEV_EVENT_EOF)
    {
//        LOGE("connection closed \n);
    }
    else if(events & BEV_EVENT_ERROR)
    {
//        printf("some other error\n");
    }
    else if(events & BEV_EVENT_CONNECTED)
    {
//        printf("已经连接了服务器.....\n");
        return ;
    }

    //释放资源
    bufferevent_free(bev);
}

Sock5Client::Sock5Client(const char *string, const char *string1) {

//    int sock;
//    if((sock=socket(AF_INET,SOCK_STREAM,0))<0)
//    {
//        LOGE("socket create failed");
//    }

    struct sockaddr_in tSockAddr;
    memset(&tSockAddr, 0, sizeof(tSockAddr));
    tSockAddr.sin_family = AF_INET;
    tSockAddr.sin_addr.s_addr = inet_addr(string);
    tSockAddr.sin_port = htons(atoi(string1));

    struct event_base* base = event_base_new();
    evutil_socket_t fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    struct bufferevent* conn = bufferevent_socket_new(base,fd,BEV_OPT_CLOSE_ON_FREE);
    bufferevent_enable(conn, EV_WRITE);
    if(bufferevent_socket_connect(conn,(struct sockaddr*)&tSockAddr,sizeof(tSockAddr)) == 0)
        printf("connect success\n");

//    bufferevent_write(conn,mesg,length);
//    bufferevent_read(conn,)

}

int Sock5Client::NewNegotiationRequest() {
    return 0;
}

int Sock5Client::Negotiate() {
    return 0;
}
