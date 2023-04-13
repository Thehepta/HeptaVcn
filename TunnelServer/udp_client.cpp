#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <csignal>
#include "tool.h"
#include <iostream>
#include <event2/event.h>
#include <event2/bufferevent.h>
#define MAX_LEN 1000
#define PORT 55559

using namespace std;
int str_to_number(const char* str);


void __on_error(evutil_socket_t sockfd, short event, void* arg) {

    cout<<"__on_error"<< event<<endl;
    exit(0);
//    struct bufferevent *partner = static_cast<bufferevent *>(ctx);

}


void *UDPHandle(void *arg) {
    struct sockaddr_in *ser_addr = static_cast<sockaddr_in *>(arg);                                //是用于指定对方(目的主机)信息
    cout<<ser_addr->sin_port<<endl;
    cout<<inet_ntoa(ser_addr->sin_addr)<<endl;
    int socket_fd; socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    socklen_t server_len = sizeof(*ser_addr);
//    if (connect(socket_fd, (struct sockaddr *) ser_addr, server_len)) {
//        cout<< errno<< "connect failed"<<endl;
//    }
    string str;
    char buff[1024];
    while (1){
        cin>>str;
        cout<<str;
        sendto(socket_fd, str.c_str(), str.length(), 0, (struct sockaddr*)ser_addr, server_len);

    }
}

int main(int argc, char** argv)
{
    struct sockaddr_in  remote;
    int sock_fd, net_fd, optval = 1;

    if ( (sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(1);
    }

    memset(&remote, 0, sizeof(remote));
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = inet_addr("127.0.0.1");
    remote.sin_port = htons(PORT);

    /* connection request */
    if (connect(sock_fd, (struct sockaddr*) &remote, sizeof(remote)) < 0) {
        perror("connect()");
        exit(1);
    }
    srand((unsigned)time(NULL));
    int ret = rand();
    write_int(sock_fd,ret);
    if(ret == read_int(sock_fd)){
        cout<<"successful"<<endl;
    }
    int port =  read_int(sock_fd);
    struct sockaddr_in ser_addr;                                //是用于指定对方(目的主机)信息

    ser_addr.sin_family = AF_INET;                                //设置为IPV4通信
    ser_addr.sin_addr.s_addr = inet_addr("127.0.0.1");                //设置目的ip
    ser_addr.sin_port = htons(port);            //设置目的端口去链接服务器

    pthread_t tid_udp;
    pthread_create(&tid_udp, NULL, UDPHandle, &ser_addr);


    struct event* ev = nullptr;
    struct event_base* evbase = event_base_new();

    ev = event_new(evbase, sock_fd, EV_CLOSED, __on_error, nullptr);

    event_base_set(evbase, ev);
    event_add(ev, nullptr);
    event_base_dispatch(evbase);

    close(sock_fd);

}



int main1(int argc, char** argv)
{
    int sk;
    char buf[MAX_LEN];
    struct sockaddr_in ser_addr;                                //是用于指定对方(目的主机)信息
    struct sockaddr_in loc_addr;                                //可以用来指定一些本地的信息，比如指定端口进行通信，而不是让系统随机分配
    int ser_addr_len,loc_addr_len;
    int ret,count;
    struct in_addr addr;

    if (argc != 3)
    {
        printf("Error: the number of args must be 3\n");
        exit(0);
    }

    //配置服务器信息
    bzero(&ser_addr, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;                                //设置为IPV4通信
    ser_addr.sin_addr.s_addr = inet_addr(argv[1]);                //设置目的ip
    ser_addr.sin_port = htons(str_to_number(argv[2]));            //设置目的端口去链接服务器
    ser_addr_len = sizeof(ser_addr);

    sk = socket(AF_INET, SOCK_DGRAM, 0);                        //设置UDP报文传输    0表示默认    SOCK_DGRAM 默认使用UDP
    //其中第三位 0 是调用方式标志位，设置socket通方式，比如非阻塞
    if(sk<0)
    {
        printf("socket create failure\n");
        return -1;
    }


    for (;;)
    {
        printf("Input info:>>>");
        scanf("%s", buf);
        if (!strcmp(buf, "quit"))
            break;
        sendto(sk, buf, sizeof(buf), 0, (struct sockaddr*)&ser_addr, ser_addr_len);

        count = recvfrom(sk, buf, sizeof(buf), 0, (struct sockaddr*)&loc_addr,
                         reinterpret_cast<socklen_t *>(&loc_addr_len));
        if (count==-1)
        {
            printf("receive data failure\n");
            return -1;
        }
        addr.s_addr = loc_addr.sin_addr.s_addr;
        printf("Receive info: %s from %s %d sendto %d\n", buf,inet_ntoa(addr),htons(loc_addr.sin_port),htons(ser_addr.sin_port));

    }

    printf("communicate end\n");
    close(sk);
    return 0;
}

int str_to_number(const char* str)
{
    int i,len, num = 0;
    len= strlen(str);

    for (i = 0; i < len;i++)
        num = num * 10 + str[i] - '0';

    return num;
}