#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>

#include <csignal>
#include <unistd.h>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <iostream>
#include <fcntl.h>
#include "Method.hpp"
#include "Client.hpp"
#include "ServerConfig.hpp"
#include "HttpRequest.hpp"
#include <algorithm>
#define MAX_EVENTS 10000
#define BUF_SIZE 5000
#define TIMEOUT_CHECK_INTERVAL 2 //s
#define INACTIVITY_TIMEOUT 10 //s

typedef struct T_sockaddr
{
    struct sockaddr_in address;
    int nbr_server;
    int port;
    std::string host; 
} s_sockaddr;

class Server
{
private:
    int create_socket(void);
    struct sockaddr_in init_sockaddr(int port, std::string host);
    int bind_socket(int serversocket_fd, struct sockaddr_in address);
    int listen_on_socket(int serversocket_fd);
    bool handleClient(int clientSocket);
    bool validateRequest(HttpRequest &request, int clientSocket, const ServerConfig &server);
    const RouteConfig *matchURI(const std::string &uri, const std::vector<RouteConfig> &locations);
    void sendRedirectResponse(int clientSocket, const std::string &redirectPath);
    bool isAllowedMethod(std::string requestMethod, const RouteConfig &Location);
    bool setNonBlocking(int sockfd);
    std::map<int, s_sockaddr> create_server(std::vector<ServerConfig> &servers);
    void accept_new_connection_request(int fd, int server_fd, struct epoll_event ev);
    std::map<int, Client *> client;
    void handleMethod(int clientSocket, ServerConfig server);
    void    _checkTimeouts(time_t currentTime,std::vector<ServerConfig> &servers,std::map<int, s_sockaddr> server_map);
public:
    ~Server();
    void start_server(std::vector<ServerConfig> &servers);
};

#endif