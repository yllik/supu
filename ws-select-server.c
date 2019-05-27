#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#define MAX_EVENTS 500

int g_epollFd;
int main(int argc, char **argv){
    //默认端口
    unsigned  short port = 4500;
    
    if(argc == 2){
        port = atoi(argv[1]);
    }

    //监听socket
    int listenFd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);
    if(listenFd == -1){
        printf("create listen socket fail %d\n", listenFd);
        return 0; 
    }
    
    //绑定端口
    struct sockaddr_in bindAddr;
    //初始化结构体
    bzero(&bindAddr, sizeof(bindAddr)); //设置结构体成员值
    bindAddr.sin_family = AF_INET;
    bindAddr.sin_port = htons(port);
    bindAddr.sin_addr.s_addr = INADDR_ANY;
    if(bind(listenFd, (struct sockaddr *)&bindAddr, sizeof(bindAddr)) != 0){
        printf("listenFd bind addr fail\n");
        return 1;        
    }

    //开始监听
    if(listen(listenFd, 500) != 0){
        printf("listen start fail\n");
        return 2;
    }
    
    //客户端地址
    struct sockaddr_in clientAddr;
    socklen_t client_addr_size = sizeof(struct sockaddr_in);
    printf("server listen on %d start!\n", port);
    
    fd_set rfds, cpy_rfds;
    struct timeval tv;
    int retval;
    int fds[1024];
    int fdsCnt = 0;
    int maxFd = listenFd;
    FD_ZERO(&cpy_rfds);
    FD_SET(listenFd, &cpy_rfds);
    char buf[64];
    
    //接受客户端连接
    while(1){
        //set timout
        tv.tv_sec = 2;
        tv.tv_usec = 0;
       
        rfds = cpy_rfds;        
 
        retval = select(maxFd+1, &rfds, NULL, NULL, &tv);
        if (retval == -1)
            perror("select()");
        else if (retval){
            int i = 0;
            for(i = 0; i< maxFd+1; i++){
                if(FD_ISSET(i, &rfds)){
                    if(i == listenFd){
                        int clientFd = accept(listenFd, (struct sockaddr *)&clientAddr, &client_addr_size);
                        if(clientFd != -1 && (errno != EAGAIN)){
                            printf("new conn is comming\n");
                            send(clientFd, "hello\n", 5, 0);
                            fds[fdsCnt] = clientFd;
                            fdsCnt++;
                            FD_SET(clientFd, &cpy_rfds);
                            if(clientFd > maxFd) maxFd = clientFd;
                        }
                        FD_SET(listenFd, &cpy_rfds);
                    }else{
                        int len = 0;
                        len = recv(i, buf, 64,0);
                        if(len == 0){
                            FD_CLR(i, &cpy_rfds);
                            close(i);
                            printf("close client:%d", i);
                        }else{
                            send(i, buf, len, 0);
                            FD_SET(i, &cpy_rfds);
                        }
                    }
                }
            }
        }
        else
            printf("No data within five seconds.\n");
    }

    printf("socket connect on prot %d\n", clientAddr.sin_port);
    printf("socket teminated on prot %d\n", port);
    
    close(listenFd);

    return 0;
}
