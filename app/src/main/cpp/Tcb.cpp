//
// Created by chic on 2023/3/4.
//

#include "Tcb.h"

std::map<std::string, Tcb*> Tcb::tcbCache = std::map<std::string, Tcb*>();

Tcb *Tcb::getTCB(std::string ipAndPort) {
    return tcbCache[ipAndPort];
}

void Tcb::putTCB(std::string ipAndPort, Tcb * tcb) {
    tcbCache.emplace(ipAndPort, tcb);
}
