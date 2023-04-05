//
// Created by chic on 2023/3/9.
//

#include "utils.h"




/***************************************/
/*   IP Public Functions               */
/***************************************/
int chksum16(void *buf1, int len, int chksum, char complement) {
    unsigned short * buf = static_cast<unsigned short *>(buf1);
    int chksum16;

    while(len > 0) {
//        LOGE("chksum len: 0x%d\r\n",len);

        if (len == 1)
            chksum16 = ((*buf)&0x00FF);
        else
            chksum16 = (*buf);

        chksum = chksum + htons(chksum16);
        *buf++;
        len -=2;
    }
//    LOGE("chksum16: return 0x%d\r\n",chksum);

    if (complement)
        return (~(chksum + ((chksum & 0xFFFF0000) >> 16))&0xFFFF);
    return chksum;
}