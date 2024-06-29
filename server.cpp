#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "util.h"

int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error!!");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8080);

    errif(bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket bind error!!");

    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error!!"); //Start Listening

    /* 
     * Accept one client connection => use accept() function
     */

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    bzero(&client_addr, sizeof(client_addr));

    int client_sockfd = accept(sockfd, (sockaddr*)&client_addr, &client_addr_len);
    errif(client_sockfd == -1, "socket accept error!!");

    printf("new client fd %d! IP: %s Port: %d \n", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    while(true){
        char buffer[1024];
        bzero(&buffer, sizeof(buffer));
        ssize_t read_bytes = read(client_sockfd, buffer, sizeof(buffer));

        if(read_bytes > 0){
            printf("Client fd %d says: %s\n",client_sockfd, buffer);
            write(client_sockfd, buffer, sizeof(buffer));
        }else if(read_bytes == 0){
            printf("Client fd %d disconnected!\n", client_sockfd);
            close(client_sockfd);
            break;
        }else{
            close(client_sockfd);
            errif(true, "socket read error!!");
        }

    }
    close(sockfd);



    return 0;
}