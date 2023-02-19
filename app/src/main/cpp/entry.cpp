#include <jni.h>
#include <string>
#include <unistd.h>
#include <android/log.h>
#include <event2/event.h>

#define LOG_TAG "theptavpn"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


int tun_interface=-1;
void *ThreadFun(void *arg)
{
    struct event_base * pEventBase = NULL;
    pEventBase = event_base_new();

    LOGE("open tun %d",tun_interface);

    // Allocate the buffer for a single packet.
    char packet[32767];
    while (true){
        int length = read(tun_interface, packet, sizeof(packet));
        if (length > 0) {
            LOGE("321321312");
        }
    }
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