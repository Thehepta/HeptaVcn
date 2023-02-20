#include <jni.h>
#include <string>
#include <unistd.h>
#include <android/log.h>
#include <event2/event.h>
#include "event2/bufferevent.h"
#include "event2/buffer.h"

#define LOG_TAG "theptavpn"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


//读回调处理  服务的发送数据会自动调用这个函数

void read_callback(struct bufferevent * pBufEv, void * pArg){
    //获取输入缓存
    struct evbuffer * pInput = bufferevent_get_input(pBufEv);
    //获取输入缓存数据的长度
    int nLen = evbuffer_get_length(pInput);
    //获取数据的地址
//    const char * pBody = (const char *)evbuffer_pullup(pInput, nLen);
    LOGE("READ DATA %d",nLen);

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

    return ;
}




int tun_interface=-1;
void *ThreadFun(void *arg)
{
    evutil_socket_t fd;
    struct event_base * pEventBase = event_base_new();
    int error;
    error = evutil_make_socket_nonblocking(tun_interface);
    if (error) {
        LOGE(LOG_TAG, "evutil_make_socket_nonblocking");
    }
    struct bufferevent * pBufEv = bufferevent_socket_new(pEventBase, tun_interface, 0);
    LOGE("open tun %d",tun_interface);

    bufferevent_setcb(pBufEv, read_callback, NULL, event_callback, NULL);

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