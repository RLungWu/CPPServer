#pragma once
#include <sys/epoll.h>
#include <sys/types.h>

class Epoll;

class Channel{
    private:
        Epoll *ep;
        int fd;
        u_int32_t events;
        u_int32_t revents;
        bool inEpoll;
    public:
        Channel(Epoll *_ep, int _fd);
        ~Channel();

        void enableReading();

        int getFd();
        u_int32_t getEvents();
        u_int32_t getRevents();
        bool getInEpoll();
        void setInEpoll();

        void setRevents(u_int32_t _revents);
};