#include "channel.h"
#include "epoll.h"
#include <sys/types.h>

Channel::Channel(Epoll *_ep, int _fd): ep(_ep), fd(_fd), events(0), revents(0), inEpoll(false){}

Channel::~Channel(){}

void Channel::enableReading(){
    events = EPOLLIN | EPOLLET;
    ep->updateChannel(this);
}

int Channel::getFd(){
    return fd;
}

u_int32_t Channel::getEvents(){
    return events;
}

u_int32_t Channel::getRevents(){
    return revents;
}

bool Channel::getInEpoll(){
    return revents;
}

void Channel::setInEpoll(){
    inEpoll = true;
}

void Channel::setRevents(u_int32_t _revents){
    revents = _revents;
}
