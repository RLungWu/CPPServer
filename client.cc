#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8080);

    connect(sockfd, (sockaddr*)&server_addr, sizeof(server_addr));

    return 0;


}