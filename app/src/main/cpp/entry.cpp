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
#include "Config.h"
#include "ipReflect.h"

#define LOG_TAG "theptavpn"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

pthread_t g_Vpntid;

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

//-------------信号回调函数1-----------------
void sig_handler1(int arg)
{
    LOGE("pthread1 get signal\r\n");
    return;
}


void *ThreadFun(void *tun_interface)
{
    struct sigaction act;

    memset(&act,0,sizeof(act));						//将act的内存空间填入0
    sigaddset(&act.sa_mask,SIGQUIT);				//添加一个信号至信号集
    act.sa_handler = sig_handler1;
    sigaction(SIGQUIT,&act,NULL);					//查询或设置信号处理方式
    pthread_sigmask(SIG_BLOCK,&act.sa_mask,NULL);
    ipReflect_start((intptr_t)tun_interface);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_hepta_theptavpn_LocalVPNService_StartVpn(JNIEnv *env, jobject thiz, int interface, int proxyType) {
    // TODO: implement setTunFd()

    pthread_create(&g_Vpntid, NULL, ThreadFun, (void*)interface);
    pthread_detach(g_Vpntid);	// 将线程分离

//    switch (fork()) {
//        case -1:
//            LOGE("fork vpn process failed");
//            break;
//        case 0:
//            LOGE("fork vpn process success child pid = %d",getpid());
//            exit(0);
//            break;
//        default:
//            break;
//    }

}
extern "C"
JNIEXPORT void JNICALL
Java_com_hepta_theptavpn_LocalVPNService_stopVpn(JNIEnv *env, jobject thiz) {

    int signal_value = pthread_kill(g_Vpntid,SIGQUIT);
    if(signal_value == ESRCH)
        LOGE("the specified thread did not exists or already quit/n");
    else if(signal_value == EINVAL)
        LOGE("signal is invalid/n");
    else
        LOGE("the specified thread is alive/n");

}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_hepta_theptavpn_LocalVPNService_connect_1server(JNIEnv *env, jobject thiz,
                                                         jstring jserver_address,
                                                         jstring jserver_port) {
    // TODO: implement connect_server()
    const char * server_address = env->GetStringUTFChars(jserver_address, NULL);
    const char * server_port = env->GetStringUTFChars(jserver_port, NULL);
//    Sock5Client *sock5Client = new Sock5Client(server_address,server_port);
//    sock5Client->connect_server();

    return true;




}
extern "C"
JNIEXPORT void JNICALL
Java_com_hepta_theptavpn_LocalVPNService_startProxyServer(JNIEnv *env, jobject thiz) {
    // TODO: implement startProxyServer()


}