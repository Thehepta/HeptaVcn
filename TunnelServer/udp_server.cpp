#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>
#include "tool.h"
#include <iostream>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <linux/if_tun.h>
#include <net/if.h>
#include <event2/buffer.h>
#include <arpa/inet.h>

#define TCP_PORT 55540
#define UDP_PORT 55550


#define PORT_START 5000
#define PORT_END 50000


using namespace std;


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





void __on_error(evutil_socket_t sockfd, short event, void* arg) {

    cout<<"__on_error"<< event<<endl;
    exit(0);
}
void __on_error(struct bufferevent *bev, short what, void *ctx) {

}


int  create_socket_bind(int &ret_fd,short sin_family,int sin_addr,int default_port){

    struct sockaddr_in src_addr;
    bzero(&src_addr,sizeof (sockaddr_in));
    src_addr.sin_family          = sin_family;
    src_addr.sin_addr.s_addr     = htonl(sin_addr);

    int i_random_port=0;
    time_t t;
    char psz_port_cmd[256];
    srand((unsigned)time(&t));

    if(default_port != 0){
        src_addr.sin_port            = htons(default_port);
        if(bind(ret_fd, (sockaddr*)&src_addr, sizeof (src_addr))==0){
            cout<<"bind successful"<<endl;
            return  default_port;
        }
    }

    while(1)
    {
        i_random_port = rand() % (PORT_END - PORT_START + 1) + PORT_START;
        sprintf(psz_port_cmd, "netstat -an | grep :%d > /dev/null", i_random_port);
        if( system(psz_port_cmd))
        {
            src_addr.sin_port            = htons(i_random_port);
            if(bind(ret_fd, (sockaddr*)&src_addr, sizeof (src_addr))==0){
                cout<<"bind successful"<<endl;
                break;
            }
            continue;
        }
    }
    return i_random_port;
}

void __on_recv(struct bufferevent *read, void *ctx)
{
    struct bufferevent *write = static_cast<bufferevent *>(ctx);
    struct evbuffer *src, *dst;
    src = bufferevent_get_input(read);
    dst = bufferevent_get_output(write);
    evbuffer_add_buffer(dst, src);
}

void process(int sock_status_fd, int tun_fd) {
    cout<<"start process:"<<getpid()<<endl;
    int flag_srandom = read_int(sock_status_fd);
    write_int(sock_status_fd, flag_srandom);
    int udp_sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    int port = create_socket_bind(udp_sockfd, AF_INET, INADDR_ANY, UDP_PORT);
    write_int(sock_status_fd,port);
    struct sockaddr_in dst_addr;
    socklen_t socklen = sizeof(sockaddr_in);
    int flag_srandom2;
    int count = recvfrom(udp_sockfd, &flag_srandom2, sizeof(int), 0, (sockaddr*)&dst_addr, &socklen );
    if(count > 0 && flag_srandom == flag_srandom2){
        if(connect(udp_sockfd, (struct sockaddr *) &dst_addr, socklen) > 0){
            perror("connect() error");
            exit(0);
        } else{
            cout<<"udp recvform ip:"<<inet_ntoa(dst_addr.sin_addr)<<endl;
            cout<<"udp recvform port:"<<dst_addr.sin_port<<endl;

        }
    }
    struct event_base* evbase = event_base_new();


    struct bufferevent *Tun_BufEv, *UdpBufEv;
    struct event* ev_tcp = nullptr;

    UdpBufEv = bufferevent_socket_new(evbase, udp_sockfd,
                                         BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);

    Tun_BufEv = bufferevent_socket_new(evbase, tun_fd,
                                      BEV_OPT_CLOSE_ON_FREE|BEV_OPT_DEFER_CALLBACKS);

    bufferevent_setcb(UdpBufEv, __on_recv, NULL, __on_error, Tun_BufEv);
    bufferevent_setcb(Tun_BufEv, __on_recv, NULL, __on_error, UdpBufEv);
    ev_tcp = event_new(evbase, sock_status_fd, EV_CLOSED, __on_error, nullptr);

    bufferevent_enable(UdpBufEv, EV_READ | EV_PERSIST);
    bufferevent_enable(Tun_BufEv, EV_READ | EV_PERSIST);
    event_base_set(evbase, ev_tcp);
    event_add(ev_tcp, nullptr);
    event_base_dispatch(evbase);
    event_base_free(evbase);


}


int main(int argc, char** argv){

    char if_name[IFNAMSIZ] = "tun111";
    int sock_fd, tun_fd ,net_fd,option;


    struct sockaddr_in local,client;
    socklen_t remotelen;

    if ((tun_fd = tun_alloc(if_name, IFF_TUN | IFF_NO_PI)) < 0 ) {
        cout<<"Error connecting to tun/tap interface:"<<if_name;
        exit(1);
    }

    /* avoid EADDRINUSE error on bind() */
    if ( (sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(1);
    }

    int port = create_socket_bind(sock_fd,AF_INET,INADDR_ANY,TCP_PORT);
    cout<<"port:"<<port<<endl;
    if (listen(sock_fd, 5) < 0) {
        perror("listen()");
        exit(1);
    }


    while (1){
        if ((net_fd = accept(sock_fd, (struct sockaddr*)&client, &remotelen)) < 0) {
            perror("accept()");
            exit(1);
        }
        int pid = fork();
        if (!pid) {
            process(net_fd, tun_fd);
        }
    }
}