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
}