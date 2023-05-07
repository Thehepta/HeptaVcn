

#include <android/log.h>
#include <string>
#include <jni.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <bits/ioctl.h>
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>
#include <net/ethernet.h>

#define LOG_TAG "checkVpn Native"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

using namespace std;


struct sockaddr_ll

{

    unsigned short int sll_family; /* 一般为AF_PACKET */
    unsigned short int sll_protocol; /* 上层协议 */
    int sll_ifindex; /* 接口类型 */
    unsigned short int sll_hatype; /* 报头类型 */
    unsigned char sll_pkttype; /* 包类型 */
    unsigned char sll_halen; /* 地址长度 */
    unsigned char sll_addr[8]; /* MAC地址 */

};


//mac地址好像有点问题
extern "C"
JNIEXPORT void JNICALL
Java_com_hepta_checkvpn_MainActivity_getifaddrs(JNIEnv *env, jobject thiz) {
    // TODO: implement checkvpn()


    struct ifaddrs *ifc, *ifc1;
    char ip[64] = {};
    char nm[64] = {};

    if(0 != getifaddrs(&ifc)) return ;
    ifc1 = ifc;

    LOGE("iface\tIP address\tNetmask\n");
    for(; NULL != ifc; ifc = (*ifc).ifa_next){
        LOGE("%s", (*ifc).ifa_name);
        if(NULL != (*ifc).ifa_addr) {
            inet_ntop(AF_INET, &(((struct sockaddr_in*)((*ifc).ifa_addr))->sin_addr), ip, 64);
            LOGE("\t%s", ip);
        }else{
            LOGE("\t\t");
        }
        if(NULL != (*ifc).ifa_netmask){
            inet_ntop(AF_INET, &(((struct sockaddr_in*)((*ifc).ifa_netmask))->sin_addr), nm, 64);
            LOGE("\t%s", nm);
        }else{
            LOGE("\t\t");
        }
        if(NULL != (*ifc).ifa_netmask){
            inet_ntop(AF_INET, &(((struct sockaddr_in*)((*ifc).ifa_netmask))->sin_addr), nm, 64);
            LOGE("\t%s", nm);
        }else{
            LOGE("\t\t");
        }
        struct sockaddr_ll *ll_addr = (struct sockaddr_ll *)ifc->ifa_addr;

//        LOGE("MAC address: %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n",  //这个可能是是ipv6地址
//             ll_addr->sll_addr[4], ll_addr->sll_addr[5], ll_addr->sll_addr[6], ll_addr->sll_addr[7], ll_addr->sll_addr[8], ll_addr->sll_addr[9],ll_addr->sll_addr[10],ll_addr->sll_addr[11],ll_addr->sll_addr[12],ll_addr->sll_addr[13]);

        LOGE("\n");
    }
    //freeifaddrs(ifc);
    freeifaddrs(ifc1);


}


//无效
extern "C"
JNIEXPORT void JNICALL
Java_com_hepta_checkvpn_MainActivity_getMac(JNIEnv *env, jobject thiz) {
    int sock_mac;

    struct ifreq ifr_mac;
    char mac_addr[30];

    sock_mac = socket( AF_INET, SOCK_STREAM, 0 );
    if( sock_mac == -1)
    {
        LOGE("create socket falise...mac/n");
        return ;
    }

    memset(&ifr_mac,0,sizeof(ifr_mac));
    strncpy(ifr_mac.ifr_name, "wlan0", sizeof(ifr_mac.ifr_name)-1);

    if( (ioctl( sock_mac, SIOCGIFHWADDR, &ifr_mac)) < 0)
    {
        LOGE("mac ioctl error/n");  //这里报错
        return ;
    }

    sprintf(mac_addr,"%02x%02x%02x%02x%02x%02x",
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[0],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[1],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[2],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[3],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[4],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[5]);

    LOGE("local mac:%s /n",mac_addr);

    close( sock_mac );
}



//失败
int get_mac_address(char *if_name, unsigned char *mac_addr) {
    if (!if_name || !mac_addr) {
        return -1;
    }
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/net/%s/address", if_name);   //没有权限读文件
    FILE *fp = fopen(path, "r");
    if (!fp) {
        return -1;
    }
    int ret = fscanf(fp, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                     &mac_addr[0], &mac_addr[1], &mac_addr[2],
                     &mac_addr[3], &mac_addr[4], &mac_addr[5]);
    fclose(fp);
    return (ret == 6) ? 0 : -1;
}



extern "C"
JNIEXPORT void JNICALL
Java_com_hepta_checkvpn_MainActivity_getMac2(JNIEnv *env, jobject thiz) {
    unsigned char mac_addr[6] = {0};
    if (get_mac_address("wlan0", mac_addr) < 0) {
        LOGE("Failed to get MAC address\n");
        return ;
    }

    LOGE("MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
}

