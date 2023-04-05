//
// Created by chic on 2023/3/4.
//

#include "Tcb.h"

std::map<int, Tcb*> Tcb::tcbCache = std::map<int, Tcb*>();

Tcb *Tcb::getTCB(int ipAndPort) {

//    return tcbCache[ipAndPort]; 这样用，稍微有点问题
// c++ 的map 使用 []取值的时候，如果没有这个键值对，会自动插入一个这个键值对，值会为空，应该是为了方便可以直接使用这样的方式赋值
    auto iterator =  tcbCache.find(ipAndPort);
    if (tcbCache.end() != iterator)
    {
        return iterator->second;
    }
    return nullptr;
}

void Tcb::putTCB(int ipAndPort, Tcb * tcb) {
    tcbCache.emplace(ipAndPort, tcb);
}

Tcb::Tcb(int socket,int status) {
    m_iSocketClient = socket;
    m_status = status;
}

int Tcb::getStatus() {
    return m_status;
}

void Tcb::setStatus(int status) {
    m_status = status;
}

int Tcb::getSocket() {
    return m_iSocketClient;
}
