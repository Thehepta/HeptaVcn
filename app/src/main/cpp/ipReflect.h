//
// Created by chic on 2023/4/9.
//


#include <android/log.h>
#include <jni.h>

#define LOG_TAG "ipreflect"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


#define CREATE_SOCKET_ERROR -1
#define CONNECT_SERVER_ERROR -1
#define CONNECT_TUNNEL_ERROR -1
#define CREATE_SOCKET_ERROR -1
#define CREATE_SOCKET_ERROR -1
#define CREATE_SOCKET_ERROR -1
#define CREATE_SOCKET_ERROR -1

extern int ipReflect_start(int fd, char *pJstring, int i, int i1);

extern int ipReflect_stop();


