#include <jni.h>
#include <string>
#include <unistd.h>
#include <android/log.h>
#include <event2/event.h>
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "arpa/inet.h"
#include "Sock5Client.h"
#include "IP.h"

#define LOG_TAG "theptavpn"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)



#define MAX_OUTPUT (512*1024)

static void __on_drained(struct bufferevent *bev, void *ctx);
static void __on_recv(struct bufferevent *bev, void *ctx);
static void __on_close(struct bufferevent *bev, void *ctx);
static void __on_error(struct bufferevent *bev, short what, void *ctx);


struct event_base * pEventBase = event_base_new();
int tun_interface=-1;



// 内存查看
void hexDump(const unsigned char *data, size_t size)
{
    int i;
    size_t offset = 0;
    while (offset < size)
    {
        LOGE("%04x  ", offset);
        for (i=0; i<16; i++)
        {
            if (i%8 == 0) putchar(' ');
            if (offset+i < size)
            {
                LOGE("%02x ", data[offset + i]);
            }
            else
            {
                LOGE("   ");
            }
        }
        LOGE("   ");
        for (i=0; i<16 && offset+i<size; i++)
        {
            if (isprint(data[offset+i]))
            {
                LOGE("%c", data[offset+i]);
            }
            else
            {
                putchar('.');
            }
        }
        putchar('\n');
        offset += 16;
    }
}



//读回调处理  服务的发送数据会自动调用这个函数

void read_callback(struct bufferevent * pBufEv, void * pArg){

    int sock =(intptr_t)pArg;
    //获取输入缓存
    struct evbuffer * pInput = bufferevent_get_input(pBufEv);

    //获取输入缓存数据的长度
    int nLen = evbuffer_get_length(pInput);
    //获取数据的地址
    char * data = new char[nLen];
    int read_len = bufferevent_read(pBufEv,data,nLen);
    IP * packet = new IP(data,read_len);
    if(read_len != nLen){
        LOGE("bufferevent_read error");
    }
    uint8_t *ret_data = packet->getData();
    if(ret_data != nullptr){
        //写到输出缓存,由bufferevent的可写事件读取并通过fd发送
        if(bufferevent_write(pBufEv, packet->getData(),packet->getDataLength())== -1){
//    if(bufferevent_write(pBufEv, data,read_len)== -1){
            LOGE("bufferevent_write error");
        } else{
            LOGE("bufferevent_write successful");
        }
    }

    return ;
}

void __on_close(struct bufferevent *bev, void *ctx)
{
    LOGE("__on_close");

    struct evbuffer *b = bufferevent_get_output(bev);

    if (evbuffer_get_length(b) == 0) {
        LOGE("Close %2d done\n", bufferevent_getfd(bev));
        bufferevent_free(bev);
    }
}
void __on_send(struct bufferevent *bev, void *ctx)
{
    struct bufferevent *partner = static_cast<bufferevent *>(ctx);
    struct evbuffer *src, *dst;
    size_t len;
    src = bufferevent_get_input(bev);
    len = evbuffer_get_length(src);
    if(len == 0){
        return;
    }
    if (!partner) {
        evbuffer_drain(src, len);
        return;
    }
    LOGE("__on_send len is %d \n", len);

    dst = bufferevent_get_output(partner);
    evbuffer_add_buffer(dst, src);

    if (evbuffer_get_length(dst) >= MAX_OUTPUT) {
        /* We're giving the other side data faster than it can
         * pass it on.  Stop reading here until we have drained the
         * other side to MAX_OUTPUT/2 bytes. */
        LOGE("%d is full\n", bufferevent_getfd(bev));
        bufferevent_setcb(partner, __on_recv, __on_drained, __on_error, bev);
        bufferevent_setwatermark(partner, EV_WRITE, MAX_OUTPUT/2, MAX_OUTPUT);
        bufferevent_disable(bev, EV_READ);
    }
}

void __on_recv(struct bufferevent *bev, void *ctx)
{
    struct bufferevent *partner = static_cast<bufferevent *>(ctx);
    struct evbuffer *src, *dst;
    size_t len;
    src = bufferevent_get_input(bev);
    len = evbuffer_get_length(src);
    if(len == 0){
        return;
    }
    if (!partner) {
        evbuffer_drain(src, len);
        return;
    }
    LOGE("__on_recv len is %d \n", len);

    dst = bufferevent_get_output(partner);
    evbuffer_add_buffer(dst, src);

    if (evbuffer_get_length(dst) >= MAX_OUTPUT) {
        /* We're giving the other side data faster than it can
         * pass it on.  Stop reading here until we have drained the
         * other side to MAX_OUTPUT/2 bytes. */
        LOGE("%d is full\n", bufferevent_getfd(bev));
        bufferevent_setcb(partner, __on_recv, __on_drained, __on_error, bev);
        bufferevent_setwatermark(partner, EV_WRITE, MAX_OUTPUT/2, MAX_OUTPUT);
        bufferevent_disable(bev, EV_READ);
    }
}

static void __on_drained(struct bufferevent *bev, void *ctx)
{
    LOGE("__on_drained");

    struct bufferevent *partner = static_cast<bufferevent *>(ctx);

    printf("%d no full\n", bufferevent_getfd(bev));
    /* We were choking the other side until we drained our outbuf a bit.
     * Now it seems drained. */
    bufferevent_setcb(bev, __on_recv, NULL, __on_error, partner);
    bufferevent_setwatermark(bev, EV_WRITE, 0, 0);
    if (partner)
        bufferevent_enable(partner, EV_READ);
}

void __on_error(struct bufferevent *bev, short what, void *ctx)
{
    LOGE("__on_error");

    struct bufferevent *partner = static_cast<bufferevent *>(ctx);

    if (what & (BEV_EVENT_EOF|BEV_EVENT_ERROR)) {
        if (what & BEV_EVENT_ERROR) {
            if (errno)
                LOGE("err = %s\n", strerror(EVUTIL_SOCKET_ERROR()));
        }

        if (partner) {
            /* Flush all pending data */
            __on_recv(bev, ctx);

            if (evbuffer_get_length(bufferevent_get_output(partner))) {
                /* We still have to flush data from the other
                 * side, but when that's done, close the other
                 * side. */
                bufferevent_setcb(partner, NULL, __on_close, __on_error, NULL);
                bufferevent_disable(partner, EV_READ);
            } else {
                /* We have nothing left to say to the other
                 * side; close it. */
                printf("Close %2d & %2d\n", bufferevent_getfd(partner), bufferevent_getfd(bev));
                bufferevent_free(partner);
            }
        }
        else {
            printf("Close %2d\n", bufferevent_getfd(bev));
        }
        bufferevent_free(bev);
    }
}


//事件回调处理
void event_callback(struct bufferevent * RemoteBufEv, short sEvent, void * pArg)
{
    //成功连接通知事件
    if(BEV_EVENT_CONNECTED == sEvent)
    {
        int error;
        error = evutil_make_socket_nonblocking(tun_interface);
        if (error) {
            LOGE(LOG_TAG, "evutil_make_socket_nonblocking");
        }
        struct bufferevent * LocalBufEv = bufferevent_socket_new(pEventBase, tun_interface, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);

        bufferevent_setcb(RemoteBufEv, __on_recv, NULL, __on_error, LocalBufEv);
        bufferevent_setcb(LocalBufEv, __on_send, NULL, __on_error, RemoteBufEv);

        bufferevent_enable(RemoteBufEv, EV_READ|EV_WRITE);
        bufferevent_enable(LocalBufEv, EV_READ|EV_WRITE);

    }
    LOGE("event_callback");
    return ;
}




void *ThreadFun(void *arg)
{
    struct bufferevent * pBufEv = NULL;

    //创建事件驱动句柄
    pEventBase = event_base_new();
    //创建socket类型的bufferevent
    pBufEv = bufferevent_socket_new(pEventBase, -1, BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);

    //设置回调函数, 及回调函数的参数
    struct sockaddr_in tSockAddr;
    memset(&tSockAddr, 0, sizeof(tSockAddr));
    tSockAddr.sin_family = AF_INET;
    tSockAddr.sin_addr.s_addr = inet_addr("192.168.31.38");
    tSockAddr.sin_port = htons(55559);

    bufferevent_setcb(pBufEv, nullptr, nullptr, event_callback, NULL);

    //连接服务器
    if( bufferevent_socket_connect(pBufEv, (struct sockaddr*)&tSockAddr, sizeof(tSockAddr)) < 0)
    {
        return 0;
    }

    //开始事件循环
    event_base_dispatch(pEventBase);
    //事件循环结束 资源清理
    bufferevent_free(pBufEv);
    event_base_free(pEventBase);







//    LOGE("open tun %d",tun_interface);
//
//    bufferevent_setcb(pBufEv, read_callback, NULL, event_callback, (void *)sock);
//    bufferevent_enable(pBufEv, EV_READ | EV_PERSIST);



    // Allocate the buffer for a single packet.
//    char packet[32767];
//    while (true){
//        int length = read(tun_interface, packet, sizeof(packet));
//        if (length > 0) {
//            LOGE("321321312");
//        }
//    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_hepta_theptavpn_LocalVPNService_setConfig(JNIEnv *env, jobject thiz, int interface,int proxyType) {
    // TODO: implement setTunFd()
    pthread_t tid;
    tun_interface = interface;
    pthread_create(&tid, NULL, ThreadFun, NULL);
    LOGE("open tun %d",tun_interface);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_hepta_theptavpn_LocalVPNService_startVpn(JNIEnv *env, jobject thiz) {
    // TODO: implement startVpn()
}
extern "C"
JNIEXPORT void JNICALL
Java_com_hepta_theptavpn_LocalVPNService_connect_1server(JNIEnv *env, jobject thiz,
                                                         jstring jserver_address,
                                                         jstring jserver_port) {
    // TODO: implement connect_server()
    const char * server_address = env->GetStringUTFChars(jserver_address, NULL);
    const char * server_port = env->GetStringUTFChars(jserver_port, NULL);
    Sock5Client *sock5Client = new Sock5Client(server_address,server_port);
//    sock5Client->connect_server();




}
extern "C"
JNIEXPORT void JNICALL
Java_com_hepta_theptavpn_LocalVPNService_startProxyServer(JNIEnv *env, jobject thiz) {
    // TODO: implement startProxyServer()


}