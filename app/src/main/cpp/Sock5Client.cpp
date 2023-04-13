//
// Created by chic on 2023/2/25.
//

#include <sys/socket.h>
#include <linux/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/tcp.h>  /* for TCP_XXX defines */
#include <vector>
#include "Sock5Client.h"
#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"

const uint8_t	ver  = 0x05;
const uint8_t	MethodNone  = 0x00;
const uint8_t	CmdConnect  = 0x00;
const uint8_t	CmdBind  = 0x00;
const uint8_t	CmdUDP  = 0x00;

void Sock5Client::socks5Config() {

}

void write_dc1(struct bufferevent *bev, void *ctx)
{
    LOGE("write_dc.....\n");

}


void read_cd1(struct bufferevent *pBufEv, void *ctx)
{

    struct evbuffer * pInput = bufferevent_get_input(pBufEv);
    int nLen = evbuffer_get_length(pInput);
    uint8_t reply[nLen];
    bufferevent_read(pBufEv,reply,nLen);
    LOGE("read_cd....%d\n",nLen);
    if(reply[0] == 0x5){
        LOGE("reply...5\n");
    }
    if(reply[1] == 0){
        LOGE("reply...0\n");
    }

}
void event_cb1(struct bufferevent *bev, short events, void * arg)
{
    if(events & BEV_EVENT_EOF)
    {
        LOGE("connection closed \n");
    }
    else if(events & BEV_EVENT_ERROR)
    {
        LOGE("some other error\n");
    }
    else if(events & BEV_EVENT_CONNECTED)
    {

        struct bufferevent* conn = static_cast<bufferevent *>(arg);
        uint8_t reply[2];
//        bufferevent_enable(bev, EV_READ|EV_WRITE);
        std::vector<uint8_t> negotiationRequest;
        negotiationRequest.push_back(ver);
        negotiationRequest.push_back(0x01);
        negotiationRequest.push_back(0x00);
        bufferevent_write(conn,negotiationRequest.data(),negotiationRequest.size());
        return ;
    }
}

Sock5Client::Sock5Client(const char *string, const char *string1) {

    memset(&server_socket, 0, sizeof(server_socket));
    server_socket.sin_family = AF_INET;
    server_socket.sin_addr.s_addr = inet_addr(string);
    server_socket.sin_port = htons(atoi(string1));
    if(connect_server()==0){

    }
}

int Sock5Client::connect_server() {

    struct event_base* base = event_base_new();
    evutil_socket_t fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    struct bufferevent* conn = bufferevent_socket_new(base,fd,BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(conn,read_cd1,write_dc1,event_cb1,conn);
    if(bufferevent_socket_connect(conn,(struct sockaddr*)&server_socket,sizeof(server_socket)) < 0){
        LOGE("connect failed\n");
        return -1;
    } else{
        LOGE("connect vpn success \n");
//        //开始事件循环
        event_base_dispatch(base);
//        //事件循环结束 资源清理
//        bufferevent_free(conn);
//        event_base_free(base);
        return 0;

    }

//    bufferevent_read(conn,)
    return 0;
}



int Sock5Client::NewNegotiationRequest() {
    return 0;
}

//int Sock5Client::Negotiate(bufferevent *conn) {
//
//
//
//    return 0;
//}
