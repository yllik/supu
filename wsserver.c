#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>

#include "shake.h"
#include "base64.h"
#include "wsdecode.h"

#define MAX_EVENTS 500
#define PORT 4500

int setnonblocking(int sockfd)
{
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1) {
        return -1;
    }
    return 0;
}
int listen(){
    /**create socket for listen*/
    int listenFd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);
    if(listenFd == -1){
        perror("create listen socket fail");
        return -1;
    }
    
    //设置网络
    struct sockaddr_in bindAddr;
    bindAddr.sin_family = AF_INET;
    bindAddr.sin_port = htons(port);
    bindAddr.sin_addr.s_addr = INADDR_ANY;
    if(bind(listenFd, (struct sockaddr *)&bindAddr, sizeof(bindAddr)) != 0){
        perror("listenFd bind addr fail");
        return -1;        
    }

    //开始监听
    if(listen(listenFd, 500) != 0){
        perror("listen start fail");
        return -1;
    }

    return listenFd;
} 

int main(int argc, char **argv){
    unsigned short port = PORT;
    if(argc == 2){
        port = atoi(argv[1]);
    }

    //监听描述符
    listenFd = listen(port);
    printf("server listen on %d start!\n", port);

    //请求地址
    struct sockaddr_in clientAddr;
    socklen_t client_addr_size = sizeof(clientAddr);
    unsigned char buf[8192];
    struct epoll_event ev,events[MAX_EVENTS];    
    int nfds, epollfd;

    epollfd = epoll_create(10);
    ev.events = EPOLLIN;
    ev.data.fd = listenFd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenFd, &ev) == -1) {
       perror("epoll_ctl: listen_sock");
       exit(EXIT_FAILURE);
    }    

    //接受客户端连接
    while(1){
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
           perror("epoll_pwait");
           _exit(EXIT_FAILURE);
        }

        int i = 0;
        unsigned char res[8092];
        
        for(i = 0; i<nfds; i++){
            memset(res, 0, sizeof(res));
            memset(buf, 0, sizeof(buf));
            if(events[i].data.fd == listenFd){
                int clientFd = accept(listenFd, (struct sockaddr *)&clientAddr, &client_addr_size);
                if(clientFd != -1 && (errno != EAGAIN)){
                    printf("new conn is comming\n");
                    setnonblocking(clientFd);
                    ev.events = EPOLLIN|EPOLLET;
                    ev.data.fd = clientFd;
                    
                    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientFd,
                                   &ev) == -1) {
                           perror("epoll_ctl: clientFd");
                           _exit(EXIT_FAILURE);
                    }
                }
            }else{
                int len = 0;
                len = recv(events[i].data.fd, buf, 8192,0);
                printf("strlen is:%d\n", len);
                if(len == 0){
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    close(events[i].data.fd);
                    printf("close client:%d", events[i].data.fd);
                }else{
                    char *get = "GET";
                    char *post = "POST";
                    if(strstr(buf, get) || strstr(buf, post)){
                        //printf("received:%s\n", buf);
                        wsshake_res(buf, res);
                    }else{
                        unsigned char payload[128];
                        memset(payload, 0, sizeof(payload));
                        get_payload(buf, payload);
                        printf("pl is:%s\n", payload);
                        set_payload(payload, res);
                        int k;
                        for(k=0; k<strlen(res); k++){
                            printf("%02x\n", res[k]);
                        }
                    }
                    size_t res_len = strlen(res);
                    char rres[res_len];
                    memset(rres, 0, res_len);
                    strncpy(rres, res, res_len);
                    printf("response:%s\n", rres);
                    size_t slen;
                    slen=send(events[i].data.fd, rres, sizeof(rres), 0);
                    printf("send len:%d, res len:%d\n", slen,  res_len);
                }
            }
        }
    }

    printf("socket connect on prot %d\n", clientAddr.sin_port);
    printf("socket teminated on prot %d\n", port);
    
    close(listenFd);

    return 0;
}
