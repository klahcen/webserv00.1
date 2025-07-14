#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "CGIHandler.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Method.hpp"
#include <ctime>
#include <sys/epoll.h>
class Client
{
    public:
        HttpRequest request;
        HttpResponse response;
        struct epoll_event ev;
        Method method;
        bool return_handelclieant;
        CGIHandler  cgi;
        time_t  start_time_client;
        int fd;

};
#endif