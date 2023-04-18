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



void *ThreadFun(void *tun_interface)
{

    ipReflect_start((intptr_t) tun_interface, nullptr, 0);
    pthread_exit(0);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_hepta_theptavpn_LocalVPNService_NativeStartVpn(JNIEnv *env, jobject thiz, int interface, jstring ipaddr,int port ,int proxyType) {
    // TODO: implement setTunFd()

    char * ipaddr_str = const_cast<char *>(env->GetStringUTFChars(ipaddr, NULL));
    ipReflect_start(interface, ipaddr_str, port);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_hepta_theptavpn_LocalVPNService_NativeStopVpn(JNIEnv *env, jobject thiz,int proxyType) {

    ipReflect_stop();

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