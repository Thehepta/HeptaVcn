//
// Created by chic on 2023/3/9.
//

#ifndef THEPTAVPN_UTILS_H
#define THEPTAVPN_UTILS_H
#include <unistd.h>
#include <sys/endian.h>
#include <android/log.h>

#if 1
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,"IPHandle",__VA_ARGS__)
#else

#define LOGE(...)

#endif


int chksum16(void *buf1, int len, int chksum, char complement) ;


















#endif //THEPTAVPN_UTILS_H
