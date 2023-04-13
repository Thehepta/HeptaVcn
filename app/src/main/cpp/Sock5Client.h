//
// Created by chic on 2023/2/25.
//
#include <jni.h>
#include <android/log.h>

#ifndef THEPTAVPN_SOCK5CLIENT_H
#define THEPTAVPN_SOCK5CLIENT_H

#define LOG_TAG "theptavpn"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)



struct NegotiationRequest {
    u_int8_t Ver;
    u_int8_t NMethods;
    u_int8_t Methods[];
};


class Sock5Client {



public:
    Sock5Client(const char *string, const char *string1);

    void socks5Config();

    int NewNegotiationRequest();
//    int Negotiate(bufferevent *pBufferevent);
    int connect_server();

private:
    struct sockaddr_in server_socket;
//    struct bufferevent* conn;

};


#endif //THEPTAVPN_SOCK5CLIENT_H
