//
// Created by chic on 2023/3/4.
//

#ifndef THEPTAVPN_TCB_H
#define THEPTAVPN_TCB_H
#include<iostream>
#include<map>


enum TCBStatus
{
    SYN_SENT,
    SYN_RECEIVED,
    ESTABLISHED,
    CLOSE_WAIT,
    LAST_ACK,
};





class Tcb {

public:
    Tcb(int socket,int status);

    static std::map<int, Tcb*> tcbCache;
    static Tcb* getTCB(int);
    static void putTCB(int,Tcb*);

    int getStatus();
    void setStatus(int status);
    int getSocket();
private:
    int m_iSocketClient;
    std::string m_ipAndPort ;
    int m_status;
};


#endif //THEPTAVPN_TCB_H
