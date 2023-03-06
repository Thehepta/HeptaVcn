//
// Created by chic on 2023/3/4.
//

#ifndef THEPTAVPN_TCB_H
#define THEPTAVPN_TCB_H
#include<iostream>
#include<map>
class Tcb {

public:
    static std::map<std::string, Tcb*> tcbCache;


    static Tcb* getTCB(std::string);
    static void putTCB(std::string,Tcb*);


};


#endif //THEPTAVPN_TCB_H
