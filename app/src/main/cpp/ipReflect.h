//
// Created by chic on 2023/4/9.
//


#include <android/log.h>

#define LOG_TAG "ipreflect"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

extern int ipReflect_start(int fd);

extern int ipReflect_stop();

