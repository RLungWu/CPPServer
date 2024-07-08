#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <vector>

#include "util.h"
#include "epoll.h"
#include "inet_address.h"
#include "socket.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL)| O_NONBLOCK);
}

void handleReadEvent(int);

int main() {
    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress("127.0.0.1",8888);
    serv_sock -> bind(serv_addr);
    serv_sock -> listen();

    Epoll *ep = new Epoll();

    serv_sock -> setnonblocking();
    //ep->addFd(serv_sock->getFd(), EPOLLIN|EPOLLET);
    
    Channel *servChannel = new Channel(ep, serv_sock -> getFd());
    servChannel -> enableReading();
    



    while (true){
        std::vector<Channel*> active_channels = ep->poll();
        int nfds = active_channels.size();
        for(int i = 0; i < nfds; ++i){
            int chfd = active_channels[i]->getFd();
            
            if(chfd == serv_sock->getFd()){ // new connection
                InetAddress *client_addr = new InetAddress();
                Socket *clnt_sock = new Socket(serv_sock->accept(client_addr));
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(client_addr->addr.sin_addr), ntohs(client_addr->addr.sin_port));
                
                clnt_sock->setnonblocking();
                Channel *clntChannel = new Channel(ep, clnt_sock->getFd());
                clntChannel -> enableReading();
            } else if(active_channels[i]->getRevents() & EPOLLIN){ //Read event
                handleReadEvent(active_channels[i]->getFd());
            }else{
                printf("unknown event\n");
            }
            
        }
    }
    delete serv_addr;
    delete serv_sock;

    return 0;
}

void handleReadEvent(int sockfd){
    char buf[READ_BUFFER];
    while(true){   
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0){
            printf("message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, sizeof(buf));
        } else if(bytes_read == -1 && errno == EINTR){  
            printf("continue reading");
            continue;
        } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
            printf("finish reading once, errno: %d\n", errno);
            break;
        } else if(bytes_read == 0){  
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd);  
            break;
        }
    }
}