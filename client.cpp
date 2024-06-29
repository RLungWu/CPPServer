#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

#include "util.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}

int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error!!");

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8080);
    

    errif(connect(sockfd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1, "socket connect error!!");

    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error!!");

    int epfd = epoll_create(0);
    errif(epfd == -1, "epoll create error!!");

    struct epoll_event events[MAX_EVENTS], ev;
    bzero(&events, sizeof(events));

    bzero(&ev, sizeof(ev));
    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET;
    setnonblocking(sockfd);
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);


    while (true) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        errif(nfds == -1, "epoll wait error!!");

        for(int i = 0; i < nfds; ++i){ //New client connecting
            if(events[i].data.fd == sockfd){
                struct sockaddr_in client_addr;
                bzero(&client_addr, sizeof(client_addr));
                socklen_t client_addr_len = sizeof(client_addr);

                int client_sockkfd = accept(sockfd, (sockaddr*)&client_addr, &client_addr_len);
                errif(client_sockkfd == -1, "socket accept error!!");
                printf("New client fd %d connected: %s:%d\n",client_sockkfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                bzero(&ev, sizeof(ev));
                ev.data.fd = client_sockkfd;
                ev.events = EPOLLIN | EPOLLET;
                setnonblocking(client_sockkfd);
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_sockkfd, &ev);
            }else if (events[i].events & EPOLLIN){ //readable event
                char buf[READ_BUFFER];
                ssize_t bytes_read = read(events[i].data.fd, buf, sizeof(buf));

                if(bytes_read > 0){
                    printf("message from client fd %d: %s\n", events[i].data.fd, buf);
                    write(events[i].data.fd, buf, sizeof(buf));
                }else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
                    printf("readable event but no data to read: %d \n", errno);
                    break;
                }else if(bytes_read == 0){
                    printf("EOF, client fd %d disconnected\n", events[i].data.fd);
                    close(events[i].data.fd);                   //close the client fd
                    break;
                }
            }else{
                //printf("unknown event: %d\n", events[i].events);
                printf("something else happened\n");
            }
    }
    close(sockfd);


    
    }
    return 0;
}