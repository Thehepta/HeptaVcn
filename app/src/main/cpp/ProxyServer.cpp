//
// Created by chic on 2023/4/5.
//
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <linux/in.h>
#include <arpa/inet.h>
#include <sys/endian.h>

int connect_server();

using namespace std;
//读缓冲区回调
void read_cb(struct bufferevent *bev, void *data)
{
    char buf[1024]={0};
    //读取数据
    bufferevent_read(bev,buf,sizeof(buf));
    cout<<"client:"<<buf<<endl;

    char *p="I am server,I have received your data";
    //写数据给客户端  给bufferevent写缓冲写数据，写缓冲将数据刷给对端，完成后回调函数
    bufferevent_write(bev,p,strlen(p)+1);
    sleep(1);

}
//写缓冲区回调
void write_cb(struct bufferevent *bev, void *data)
{
    cout<<"成功将数据写给客户端"<<endl;
}
//事件回调
void event_cb(struct bufferevent *bev, short events, void *data)
{
    if (events & BEV_EVENT_EOF)
    {
        cout<<"connection closed"<<endl;
    }
    else if(events & BEV_EVENT_ERROR)
    {
        cout<<"some other error"<<endl;
    }

    bufferevent_free(bev);
    cout<<"buffevent 资源已经被释放"<<endl;
}
//监听回调函数
void cb_listener(
        struct evconnlistener *listener,
        evutil_socket_t fd,
        struct sockaddr *addr,
        int len, void *ptr)
{
    cout<<"connect new client"<<endl;
    struct event_base*base=(struct event_base*)ptr;
    //创建bufferevent事件对象
    struct bufferevent *bev;
    bev=bufferevent_socket_new(base,fd,BEV_OPT_CLOSE_ON_FREE);

    //给bufferevent缓冲区设置回调
    bufferevent_setcb(bev,read_cb,write_cb,event_cb,NULL);
    bufferevent_enable(bev, EV_READ);
    return ;
}



void * startProxyServerThread (void *arg)
{
    int proxy_server = connect_server();
    if (proxy_server == -1){
        pthread_exit((void *) "Don't connect server");
    }
    int port =(intptr_t)arg;
    //创建地址结构并初始化
    struct sockaddr_in proxy_client;
    memset(&proxy_client, 0, sizeof(proxy_client));
    proxy_client.sin_family = AF_INET;
    proxy_client.sin_port = htons(port);
    proxy_client.sin_addr.s_addr = inet_addr("127.0.0.1");

    //创建event_base
    struct event_base *base;
    base=event_base_new();

    //创建监听器（创建套接字，绑定，接收收连接请求）
    struct evconnlistener *listener;
    //evconnlistener_new_bind()函数返回监听器对象
    listener=evconnlistener_new_bind(
            base,cb_listener,base,
            LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
            -1,
            (struct sockaddr *)&proxy_client,
            sizeof(proxy_client));

    //启动循环监听
    event_base_dispatch(base);
    //释放监听器
    evconnlistener_free(listener);
    //释放event_base
    event_base_free(base);
    return 0;

}

int connect_server() {

    return 0;
}


pthread_t startProxyServer (int port)
{
    pthread_t tid;
    pthread_create(&tid, NULL, startProxyServerThread, reinterpret_cast<void *>(port));

    return tid;

}