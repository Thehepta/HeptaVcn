/**************************************************************************
 * simpletun.c                                                            *
 *                                                                        *
 * A simplistic, simple-minded, naive tunnelling program using tun/tap    *
 * interfaces and TCP. DO NOT USE THIS PROGRAM FOR SERIOUS PURPOSES.      *
 *                                                                        *
 * You have been warned.                                                  *
 *                                                                        *
 * (C) 2010 Davide Brini.                                                 *
 *                                                                        *
 * DISCLAIMER AND WARNING: this is all work in progress. The code is      *
 * ugly, the algorithms are naive, error checking and input validation    *
 * are very basic, and of course there can be bugs. If that's not enough, *
 * the program has not been thoroughly tested, so it might even fail at   *
 * the few simple things it should be supposed to do right.               *
 * Needless to say, I take no responsibility whatsoever for what the      *
 * program might do. The program has been written mostly for learning     *
 * purposes, and can be used in the hope that is useful, but everything   *
 * is to be taken "as is" and without any kind of warranty, implicit or   *
 * explicit. See the file LICENSE for further details.                    *
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <stdarg.h>
#include <iostream>

#include <event2/bufferevent_ssl.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <cassert>

/* buffer for reading from tun/tap interface, must be >= 1500 */
#define BUFSIZE 2000
#define CLIENT 0
#define SERVER 1
#define PORT 55559
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;
using namespace std;
typedef struct tIPPackHead
{

    BYTE ver_hlen;      //IP协议版本和IP首部长度。高4位为版本，低4位为首部的长度(单位为4bytes)
    BYTE byTOS;       //服务类型
    WORD wPacketLen; //IP包总长度。包括首部，单位为byte。[Big endian]
    WORD wSequence;    //标识，一般每个IP包的序号递增。[Big endian]

    union
    {
        WORD Flags; //标志
        WORD FragOf;//分段偏移
    };
    BYTE byTTL;         //生存时间
    BYTE byProtocolType; //协议类型，见PROTOCOL_TYPE定义
    WORD wHeadCheckSum;    //IP首部校验和[Big endian]
    DWORD dwIPSrc;         //源地址
    DWORD dwIPDes;         //目的地址
    //BYTE Options;          //选项
} IP_HEAD;


int debug;
char *progname;
int cnt;
struct event_base *base;
int tap_fd;

int DecodeIP(char *buf, int len)
{
    int n = len;
    if (n >= sizeof(IP_HEAD))
    {
        IP_HEAD iphead;
        iphead = *(IP_HEAD*)buf;

        cout << "第 "<<cnt++<<" 个IP数据包信息：" << endl;


        cout << "协议版本:" <<(iphead.ver_hlen >> 4) << endl;
        cout << "首部长度:" << ((iphead.ver_hlen & 0x0F) << 2) << endl;//单位为4字节
        cout << "服务类型:Priority: " << (iphead.byTOS >> 5) << ",Service: " << ((iphead.byTOS >> 1) & 0x0f) << endl;
        cout << "IP包总长度:" << ntohs(iphead.wPacketLen) << endl; //网络字节序转为主机字节序
        cout << "标识:" << ntohs(iphead.wSequence) << endl;
        cout << "标志位:" << "DF=" << ((iphead.Flags >> 14) & 0x01) << ",MF=" << ((iphead.Flags >> 13) & 0x01) << endl;
        cout << "片偏移:" << (iphead.FragOf & 0x1fff) << endl;
        cout << "生存周期:" << (int)iphead.byTTL << endl;
        cout << "协议类型:" << int(iphead.byProtocolType) << endl;
        cout << "首部校验和:" << ntohs(iphead.wHeadCheckSum) << endl;
        cout << "源地址:" << inet_ntoa(*(in_addr*)&iphead.dwIPSrc) << endl;
        cout << "目的地址:" << inet_ntoa(*(in_addr*)&iphead.dwIPDes) << endl;

        cout << "==============================================================" << endl << endl;
    }else{
        cout << "***包长不足：" << n << " < " << sizeof(IP_HEAD) << endl;
    }

    return 0;
}

/**************************************************************************
 * tun_alloc: allocates or reconnects to a tun/tap device. The caller     *
 *            must reserve enough space in *dev.                          *
 **************************************************************************/
int tun_alloc(char *dev, int flags) {

    struct ifreq ifr;
    int fd, err;
    char *clonedev = "/dev/net/tun";

    if( (fd = open(clonedev , O_RDWR)) < 0 ) {
        perror("Opening /dev/net/tun");
        return fd;
    }

    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = flags;

    if (*dev) {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }

    if( (err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0 ) {
        perror("ioctl(TUNSETIFF)");
        close(fd);
        return err;
    }

    strcpy(dev, ifr.ifr_name);

    return fd;
}

/**************************************************************************
 * cread: read routine that checks for errors and exits if an error is    *
 *        returned.                                                       *
 **************************************************************************/
int cread(int fd, char *buf, int n){
    int nread;

    if((nread=read(fd, buf, n)) < 0){
        perror("Reading data");
        exit(1);
    }
    //DecodeIP(buf, nread);


//    printf("read len:%d -> ", nread);
//    for(int i = 0;i < nread;i++){
//        printf("%x",buf[i]);
//    }
//    printf("\n");
    return nread;
}

/**************************************************************************
 * cwrite: write routine that checks for errors and exits if an error is  *
 *         returned.                                                      *
 **************************************************************************/
int cwrite(int fd, char *buf, int n){

    int nwrite;
    if((nwrite=write(fd, buf, n)) < 0){
        perror("Writing data");
        exit(1);
    }

//    printf("write: ");
//    for(int i = 0;i < n;i++){
//        printf("%x",buf[i]);
//    }
//    printf("\n");
    return nwrite;
}

/**************************************************************************
 * read_n: ensures we read exactly n bytes, and puts them into "buf".     *
 *         (unless EOF, of course)                                        *
 **************************************************************************/


int read_n(int fd, char *buf, int n) {

    int nread, left = n;

    while(left > 0) {
        if ((nread = cread(fd, buf, left)) == 0){
            return 0 ;
        }else {
            left -= nread;
            buf += nread;
        }
    }
    return n;
}

// 读取ipv4包头，获得需要继续读的长度
int read_ipv4_len_left(int fd ,char *buf){
    cout << "==read_ipv4_len_left==" << endl;
    int nread;
    nread = read_n(fd, buf, sizeof(IP_HEAD));
    cout << "==read_ipv4_len_left== -> read_n" << nread << endl;
    if (nread = 0){
        cout << "read_ipv4_len_left get 0." << endl;
        return 0;
    }

    IP_HEAD iphead;
    iphead = *(IP_HEAD*)buf;
    DecodeIP(buf, nread);
    int len_to_read = ntohs(iphead.wPacketLen) - sizeof(IP_HEAD);
    cout << "此IP包总长度:    " << ntohs(iphead.wPacketLen) << endl;
    cout << "剩余要读取的长度:" << len_to_read << endl;
    return len_to_read;
}
/**************************************************************************
 * do_debug: prints debugging stuff (doh!)                                *
 **************************************************************************/
void do_debug(char *msg, ...){

    va_list argp;

    if(debug) {
        va_start(argp, msg);
        vfprintf(stderr, msg, argp);
        va_end(argp);
    }
}

/**************************************************************************
 * my_err: prints custom error messages on stderr.                        *
 **************************************************************************/
void my_err(char *msg, ...) {

    va_list argp;

    va_start(argp, msg);
    vfprintf(stderr, msg, argp);
    va_end(argp);
}

/**************************************************************************
 * usage: prints usage and exits.                                         *
 **************************************************************************/
void usage(void) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "%s -i <ifacename> [-s|-c <serverIP>] [-p <port>] [-u|-a] [-d]\n", progname);
    fprintf(stderr, "%s -h\n", progname);
    fprintf(stderr, "\n");
    fprintf(stderr, "-i <ifacename>: Name of interface to use (mandatory)\n");
    fprintf(stderr, "-s|-c <serverIP>: run in server mode (-s), or specify server address (-c <serverIP>) (mandatory)\n");
    fprintf(stderr, "-p <port>: port to listen on (if run in server mode) or to connect to (in client mode), default 55555\n");
    fprintf(stderr, "-u|-a: use TUN (-u, default) or TAP (-a)\n");
    fprintf(stderr, "-d: outputs debug information while running\n");
    fprintf(stderr, "-h: prints this help text\n");
    exit(1);
}


void __on_recv(struct bufferevent *bev, void *ctx)
{
    struct bufferevent *partner = static_cast<bufferevent *>(ctx);
    struct evbuffer *src, *dst;
    size_t len;
    src = bufferevent_get_input(bev);
    len = evbuffer_get_length(src);
    if(len == 0){
        return;
    }
    if (!partner) {
        evbuffer_drain(src, len);
        return;
    }
    printf("__on_recv len is %d \n", len);

    dst = bufferevent_get_output(partner);
    evbuffer_add_buffer(dst, src);
}

void Tap_Read(struct bufferevent *bev, void *ctx)
{
    struct bufferevent *partner = static_cast<bufferevent *>(ctx);
    struct evbuffer *src, *dst;
    size_t len;
    src = bufferevent_get_input(bev);
    len = evbuffer_get_length(src);
    if(len == 0){
        return;
    }
    if (!partner) {
        evbuffer_drain(src, len);
        return;
    }
    printf("__on_send len is %d \n", len);

    dst = bufferevent_get_output(partner);
    evbuffer_add_buffer(dst, src);
}


void __on_error(struct bufferevent *bev, short what, void *ctx) {

    printf("__on_error");

    struct bufferevent *partner = static_cast<bufferevent *>(ctx);

    if (what & (BEV_EVENT_EOF|BEV_EVENT_ERROR)) {
        if (what & BEV_EVENT_ERROR) {
            if (errno)
                printf("err = %s\n", strerror(EVUTIL_SOCKET_ERROR()));
        }
    }
}

static void __on_accept(struct evconnlistener *listener, evutil_socket_t fd,
                        struct sockaddr *a, int slen, void *p)
{


    struct bufferevent *Tap_BufEv, *RemoteBufEv;

    /* Create two linked bufferevent objects: one to connect, one for the
     * new connection */
    RemoteBufEv = bufferevent_socket_new(base, fd,
                                   BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);

    int error = evutil_make_socket_nonblocking(tap_fd);
    if (error) {
        printf( "evutil_make_socket_nonblocking");
    }
    Tap_BufEv = bufferevent_socket_new(base, tap_fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);

    assert(RemoteBufEv && Tap_BufEv);

//    bufferevent_setcb(RemoteBufEv, __on_recv, NULL, __on_error, LocalBufEv);
    bufferevent_setcb(Tap_BufEv, Tap_Read, NULL, __on_error, RemoteBufEv);

//    bufferevent_enable(RemoteBufEv, EV_READ | EV_WRITE);
    bufferevent_enable(Tap_BufEv, EV_READ | EV_WRITE);
}



int main(int argc, char *argv[]) {
    cout << "ip 包头:" << sizeof(IP_HEAD) << endl;
    int  option;
    int flags = IFF_TUN;
    char if_name[IFNAMSIZ] = "";
    int maxfd;
    uint16_t nread, nwrite, plength;
    char buffer[BUFSIZE];
    struct sockaddr_in local, remote;
    char remote_ip[16] = "";            /* dotted quad IP string */
    unsigned short int port = PORT;
    int sock_fd, net_fd, optval = 1;
    socklen_t remotelen;
    int cliserv = -1;    /* must be specified on cmd line */
    unsigned long int tap2net = 0, net2tap = 0;

    progname = argv[0];

    /* Check command line options */
    while((option = getopt(argc, argv, "i:sc:p:uahd")) > 0) {
        switch(option) {
            case 'd':
                debug = 1;
                break;
            case 'h':
                usage();
                break;
            case 'i':
                strncpy(if_name,optarg, IFNAMSIZ-1);
                break;
            case 's':
                cliserv = SERVER;
                break;
            case 'c':
                cliserv = CLIENT;
                strncpy(remote_ip,optarg,15);
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'u':
                flags = IFF_TUN;
                break;
            case 'a':
                flags = IFF_TAP;
                break;
            default:
                my_err("Unknown option %c\n", option);
                usage();
        }
    }

    argv += optind;
    argc -= optind;

    if(argc > 0) {
        my_err("Too many options!\n");
        usage();
    }

    if(*if_name == '\0') {
        my_err("Must specify interface name!\n");
        usage();
    } else if(cliserv < 0) {
        my_err("Must specify client or server mode!\n");
        usage();
    } else if((cliserv == CLIENT)&&(*remote_ip == '\0')) {
        my_err("Must specify server address!\n");
        usage();
    }

    /* initialize tun/tap interface */
    if ( (tap_fd = tun_alloc(if_name, flags | IFF_NO_PI)) < 0 ) {
        my_err("Error connecting to tun/tap interface %s!\n", if_name);
        exit(1);
    }
    do_debug("Successfully connected to interface %s\n", if_name);



    struct sockaddr_storage lsn_on_addr = {0};
    struct sockaddr_storage con_to_addr = {0};
    int lsn_len = sizeof(lsn_on_addr);

    int ret = -1;
    struct evconnlistener *listener = NULL;

    ret = evutil_parse_sockaddr_port("192.168.31.38:55559", (struct sockaddr*)&lsn_on_addr, &lsn_len);
    assert(0 == ret);
    base = event_base_new();
    assert(base);


    listener = evconnlistener_new_bind(base, __on_accept, NULL,
                                       LEV_OPT_CLOSE_ON_FREE | LEV_OPT_CLOSE_ON_EXEC | LEV_OPT_REUSEABLE,
                                       -1, (struct sockaddr*)&lsn_on_addr, lsn_len);

    assert(listener);
    event_base_dispatch(base);
    evconnlistener_free(listener);
    event_base_free(base);

}