#include <jni.h>
#include <string>
#include <unistd.h>
#include <android/log.h>
#include <event2/event.h>
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "arpa/inet.h"
#include "Sock5Client.h"
#define LOG_TAG "theptavpn"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


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

    int sock =(long)pArg;
    //获取输入缓存
    struct evbuffer * pInput = bufferevent_get_input(pBufEv);

    //获取输入缓存数据的长度
    int nLen = evbuffer_get_length(pInput);
    //获取数据的地址
    char * data = new char[nLen];
    int read_len = bufferevent_read(pBufEv,data,nLen);
    if(read_len != nLen){
        LOGE("bufferevent_read error");
    }
    if(send(sock, data, nLen, 0) == -1)
    {
        LOGE("send error");
    }
    LOGE("READ DATA %d",nLen);
//    hexDump(reinterpret_cast<const unsigned char *>(pBody), nLen);
    //进行数据处理

    //写到输出缓存,由bufferevent的可写事件读取并通过fd发送
    //bufferevent_write(pBufEv, pResponse, nResLen);

    return ;
}


//事件回调处理
void event_callback(struct bufferevent * pBufEv, short sEvent, void * pArg)
{
    //成功连接通知事件
    if(BEV_EVENT_CONNECTED == sEvent)
    {
        bufferevent_enable(pBufEv, EV_READ);
    }
    LOGE("event_callback");
    return ;
}


void *ThreadFun(void *arg)
{
    evutil_socket_t fd;
    int error;
    int sock;

    if((sock=socket(AF_INET,SOCK_STREAM,0))<0)
    {
        LOGE("socket create failed");
    }
    struct sockaddr_in tSockAddr;
    memset(&tSockAddr, 0, sizeof(tSockAddr));
    tSockAddr.sin_family = AF_INET;
    tSockAddr.sin_addr.s_addr = inet_addr("192.168.0.100");
    tSockAddr.sin_port = htons(8883);
    if(connect(sock,(struct sockaddr*)&tSockAddr,sizeof(tSockAddr))<0){
        LOGE("connect error");
    }

    error = evutil_make_socket_nonblocking(tun_interface);
    if (error) {
        LOGE(LOG_TAG, "evutil_make_socket_nonblocking");
    }
    struct bufferevent * pBufEv = bufferevent_socket_new(pEventBase, tun_interface, 0);
    LOGE("open tun %d",tun_interface);

    bufferevent_setcb(pBufEv, read_callback, NULL, event_callback, (void *)sock);
    bufferevent_enable(pBufEv, EV_READ | EV_PERSIST);

    //开始事件循环
    event_base_dispatch(pEventBase);
    //事件循环结束 资源清理
    bufferevent_free(pBufEv);
    event_base_free(pEventBase);

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
Java_com_hepta_theptavpn_LocalVPNService_setTunFd(JNIEnv *env, jobject thiz,int interface) {
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