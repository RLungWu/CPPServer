#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

#include "socket.h"
#include "util.h"
#include "inet_address.h"

Socket::Socket(): fd(-1){
    fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(fd == -1, "socket");
}

Socket::Socket(int _fd) : fd(_fd) {
    errif(fd == -1, "socket");
}

Socket::~Socket(){
    if(fd != -1){
        close(fd);
        fd = -1;
    }
}

void Socket::bind(const InetAddress *addr){
    errif(::bind(fd, (struct sockaddr*)&addr->addr, addr->addr_len) == -1, "Socket Bind Error");
}

void Socket::listen(){
    errif(::listen(fd, 1024) == -1, "Socket Listen Error");
}

void Socket::setnonblocking(){
    errif(fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) == -1, "Socket Set Nonblocking Error");
}

int Socket::accept(InetAddress* addr){
    int conn_fd = ::accept(fd, (struct sockaddr*)&addr->addr, &addr->addr_len);
    errif(conn_fd == -1, "Socket Accept Error");
    return conn_fd;
}

int Socket::getFd(){
    return fd;
}