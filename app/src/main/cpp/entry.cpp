#include <jni.h>
#include <string>
#include <unistd.h>
#include <android/log.h>
#include <event2/event.h>
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "arpa/inet.h"
#include "ipReflect.h"

#define LOG_TAG "theptavpn"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

extern "C"
JNIEXPORT int JNICALL
Java_com_hepta_theptavpn_Tunnel_IPreflectorTunnel_NativeStartVpn(JNIEnv *env, jobject thiz, int interface, jstring ipaddr,int port) {
    // TODO: implement setTunFd()

    char * ipaddr_str = const_cast<char *>(env->GetStringUTFChars(ipaddr, NULL));
    int ret = ipReflect_start(interface, ipaddr_str, port);
    ipReflect_stop();
    return ret;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_hepta_theptavpn_Tunnel_IPreflectorTunnel_NativeStopVpn(JNIEnv *env, jobject thiz) {

    ipReflect_stop();

}