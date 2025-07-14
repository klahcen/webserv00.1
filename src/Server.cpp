#include "../include/Server.hpp"
#include "../include/CGIHandler.hpp"
#include "../include/HttpResponse.hpp"
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <string>
#include <sys/epoll.h>
#include <vector>

int Server::create_socket(void)
{
    //AF_INET → Address Family: IPv4 (for IPv6, use AF_INET6).
    //SOCK_STREAM → Socket Type: TCP (for UDP, use SOCK_DGRAM).
    //0 → Protocol: Auto-selects the default protocol for the socket type (TCP for SOCK_STREAM).
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        std::cerr << "Socket creation failed: " << std::endl;
        return -1;
    }
    return fd;
}

struct sockaddr_in Server::init_sockaddr(int port, std::string host)
{
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &address.sin_addr) <= 0)
    {
        std::cerr << "Invalid address or unsupported host: " << host << std::endl;
        struct sockaddr_in emptyAddress;
        memset(&emptyAddress, 0, sizeof(emptyAddress));
        return emptyAddress;
    }
    return address;
}

int Server::bind_socket(int serversocket_fd, struct sockaddr_in address)
{
    int optval = 1;
    if (setsockopt(serversocket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
    {
        perror("setsockopt");
        close(serversocket_fd);
        return 1;
    }
    bind(serversocket_fd, (const struct sockaddr *)&address, sizeof(address));

    return 0;
}

int Server::listen_on_socket(int serversocket_fd)
{
    listen(serversocket_fd, 3);

    return 0;
}

std::string strtrim(std::string str)
{
    size_t start = str.find_first_not_of("/");
    if (start == std::string::npos)
    {
        return "";
    }
    size_t end = str.find_last_not_of("/");
    return str.substr(start, end - start + 1);
}

std::string get_directory1(std::string path)
{
    std::string str;
    path = strtrim(path);
    if (path == "")
        return "/";
    str = path.substr(0, path.find_last_of('/'));
    return str;
}

bool isValidURI(const std::string &uri)
{
    const std::string allowedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";

    for (std::string::const_iterator it = uri.begin(); it != uri.end(); ++it)
    {
        if (allowedChars.find(*it) == std::string::npos)
        {
            return (false);
        }
    }
    return (true); 
}

bool Server::validateRequest(HttpRequest &request, int clientSocket, const ServerConfig &server)
{
    const std::string &transferEncoding = request.getHeader("Transfer-Encoding");
    if (!transferEncoding.empty() && transferEncoding != "chunked")
    {
        HttpResponse::sendError(clientSocket, 501, "Not Implemented", server);
        return (false);
    }

    if (transferEncoding.empty() && request.getHeader("Content-Length").empty() && request.getMethod() == "POST")
    {
        HttpResponse::sendError(clientSocket, 400, "Bad Request", server);
        return (false);
    }

    const std::string &uri = request.getPath();
    if (uri.length() > 2048)
    {
        HttpResponse::sendError(clientSocket, 414, "Request-URI Too Long", server);
        return (false);
    }

    if (!isValidURI(uri))
    {
        HttpResponse::sendError(clientSocket, 400, "Bad Request: Invalid URI characters", server);
        return (false);
    }

    ssize_t maxBodySize = server.getClientMaxBodySize();
    if ((ssize_t)request.getBody().tellg() > maxBodySize)
    {
        HttpResponse::sendError(clientSocket, 413, "Request Entity Too Large", server);
        return (false);
    }

    return (true); 
}

const RouteConfig *Server::matchURI(const std::string &uri, const std::vector<RouteConfig> &locations)
{
    const RouteConfig *bestMatch = NULL;
    size_t bestMatchLength = 0;

    for (std::vector<RouteConfig>::const_iterator it = locations.begin(); it != locations.end(); ++it)
    {
        const std::string &path = it->getPath();

        if (uri.find(path) == 0)
        {
            if (path == "/" || uri.length() >= path.length() || uri[path.length()] == '/')
            {
                if (path.length() > bestMatchLength)
                {
                    bestMatch = &(*it);
                    bestMatchLength = path.length();
                }
            }
        }
    }

    return (bestMatch);
}

bool Server::isAllowedMethod(std::string requestMethod, const RouteConfig &Location)
{
    const std::vector<std::string> &allowedMethods = Location.getAllowedMethods();
    for (std::vector<std::string>::const_iterator it = allowedMethods.begin(); it != allowedMethods.end(); it++)
    {
        if (*it == requestMethod)
            return (true);
    }
    return (false);
}

void Server::sendRedirectResponse(int clientSocket, const std::string &redirectPath)
{
    HttpResponse redirectResponse(301, "Moved Permanently");
    redirectResponse.setHeader("Location", redirectPath);
    redirectResponse.setHeader("Content-Length", "0");
    redirectResponse.setHeader("Connection", "close");
    if (!redirectResponse.sendResponse(clientSocket))
        std::cerr << "Failed to send response to client" << std::endl;
}

static std::string combinePaths(const std::string &part1, const std::string &part2, const std::string &part3)
{
    std::string combinedPath = part1;

    if (!part1.empty() && part1[part1.size()-1] != '/' && !part2.empty() && part2[0] != '/')
    {
        combinedPath += '/';
    }

    combinedPath += part2;

    if (!part3.empty())
    {
        if (combinedPath[combinedPath.size()-1] != '/' && part3[0] != '/')
        {
            combinedPath += '/';
        }
        combinedPath += part3;
    }

    return combinedPath;
}

static void replaceAlias(const RouteConfig &location, HttpRequest &request, std::string &target_file)
{
    target_file = combinePaths(location.getAlias(), request.getPath().substr(location.getPath().length()), "");
}

static void appendRoot(const RouteConfig &location, HttpRequest &request, std::string &target_file)
{
    target_file = combinePaths(location.getRoot(), request.getPath(), "");
}

bool fileExists(const std::string &path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

bool isDirectory(const std::string &path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}


std::string normalizeSlashes(const std::string &path)
{
    std::string result;
    bool lastWasSlash = false;

    for (std::string::const_iterator it = path.begin(); it != path.end(); ++it)
    {
        if (*it == '/')
        {
            if (!lastWasSlash)
            {
                result += '/';
                lastWasSlash = true;
            }
        }
        else
        {
            result += *it;
            lastWasSlash = false;
        }
    }

    return result;
}


bool isMultipartRequestComplete(const std::string &body, const std::string &boundary)
{
    std::string endBoundary = "--" + boundary + "--";
    return body.find(endBoundary) != std::string::npos;
}

bool Server::handleClient(int clientSocket)
{
    HttpRequest request;
    char buffer[BUF_SIZE];
    memset(buffer, 0, BUF_SIZE);
    int bytesRead = recv(clientSocket, buffer, BUF_SIZE-1, 0);
    if (bytesRead <=  0)
    {
        client[clientSocket]->request.requestcomplet = true;
        return false;
    }
    std::string tmp(buffer, bytesRead);
    if(!client[clientSocket]->request.readFromSocket(tmp))
    {
        client[clientSocket]->request.requestcomplet = true;
        return false;
    }
    tmp.clear();
    return true;
}

std::map<int, s_sockaddr> Server::create_server(std::vector<ServerConfig> &servers)
{
    std::map<int, s_sockaddr> server_map;
    for (size_t i = 0; i < servers.size(); i++)
    {
        int server_socket_fd = create_socket();
        if (server_socket_fd == -1)
            continue;
        int port = servers[i].getPort();
        std::string host = servers[i].getHost();
        struct sockaddr_in address = init_sockaddr(port, host);

        if (bind_socket(server_socket_fd, address) == -1)
            continue;
        if (listen_on_socket(server_socket_fd) == -1)
            continue;
        s_sockaddr addr;
        addr.address = address;
        addr.nbr_server = i;
        addr.host = servers[i].getHost();
        addr.port = servers[i].getPort();
        server_map[server_socket_fd] = addr;

        std::cout << "Server initialized on port " << port << " : " << servers[i].getHost() << std::endl;
    }
    return server_map;
}

void Server::accept_new_connection_request(int fd, int server_fd, struct epoll_event ev)
{
    struct sockaddr_in new_addr;
    int addrlen = sizeof(struct sockaddr_in);
    while (1)
    {
        int conn_sock = accept(server_fd, (struct sockaddr *)&new_addr,
                               (socklen_t *)&addrlen);
 
        if (conn_sock == -1)
        {
            perror("ERROR");
            break;
        } 
        ev.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
        ev.data.fd = conn_sock;

        if (epoll_ctl(fd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
        {
            perror("epoll_ctl: conn_sock");
            break;
        }
        if(client.find(conn_sock)==client.end())
        {
            std::cout<<"New Client fd: "<< conn_sock<<"."<<std::endl;
            Client *newclient = new Client();
            client.insert(std::make_pair(conn_sock, newclient));
            client[conn_sock]->return_handelclieant = true;
            client[conn_sock]->start_time_client = time(NULL);
            client[conn_sock]->fd = fd;
            break;
        }
    }
}

std::string urlDecode(const std::string &encoded) {
    std::string decoded;
    char hex[3] = {0}; 
    size_t len = encoded.length();

    for (size_t i = 0; i < len; ++i) {
        if (encoded[i] == '%' && i + 2 < len) {
            hex[0] = encoded[i + 1];
            hex[1] = encoded[i + 2];

            char decodedChar = static_cast<char>(strtol(hex, NULL, 16));
            decoded += decodedChar;
            i += 2;
        } else if (encoded[i] == '+') {
            decoded += ' ';
        } else {
            decoded += encoded[i];
        }
    }
    return decoded;
}

void Server::handleMethod(int clientSocket, ServerConfig server)
{    
    HttpRequest &request(client[clientSocket]->request);
    client[clientSocket]->method.response_complet = true;
    client[clientSocket]->cgi.cgi_is_complet = true;

    if (!validateRequest(request, clientSocket, server))
    {
        std::cerr << "Validation failed for clientSocket: " << clientSocket << "\n";
        close(clientSocket);
        return;
    }

    std::string uri = normalizeSlashes(request.getPath());
    const RouteConfig *findLocation = matchURI(uri, server.getRoutes());

    std::string targetPath;
    if (!findLocation)
    {
        targetPath = combinePaths(server.getRoot(), request.getPath(), "");
        if (isDirectory(targetPath))
        {
            if (targetPath[targetPath.length() - 1] != '/')
            {
                HttpResponse::sendRedirect(clientSocket, request.getPath() + "/");
                close(clientSocket);
                return;
            }
            targetPath += server.getIndex();
        }
        else if (!fileExists(targetPath))
        {
            HttpResponse::sendError(clientSocket, 403, "Forbidden", server);
            close(clientSocket);
            return;
        }
        if (request.getMethod() != "GET")
        {
            HttpResponse::sendError(clientSocket, 405, "Method Not Allowed", server);
            close(clientSocket);
            return;
        }
    }
    else
    {
        if (findLocation->hasRedirect())
        {
            sendRedirectResponse(clientSocket, findLocation->getRedirection());
            close(clientSocket);
            return;
        }
        if (!isAllowedMethod(request.getMethod(), *findLocation))
        {
            HttpResponse::sendError(clientSocket, 405, "Method Not Allowed", server);
            close(clientSocket);
            return;
        }
        if (!findLocation->getAlias().empty())
            replaceAlias(*findLocation, request, targetPath);
        else
            appendRoot(*findLocation, request, targetPath);
    }

    targetPath = normalizeSlashes(targetPath);
    
    targetPath = urlDecode(targetPath);
    std::string::size_type pos = targetPath.find('?');
    std::string checkPath = targetPath;
    if (pos != std::string::npos)
    {
        checkPath = targetPath.substr(0, pos); 
    }
    if (!fileExists(checkPath) && !isDirectory(checkPath))
    {
        HttpResponse::sendError(clientSocket, 404, "Not Found", server);
        // std::cerr << "'" << checkPath << "' Not Found" << std::endl;
        close(clientSocket);
        return;
    }
    
    std::string newPath = targetPath;
    size_t queryPos = newPath.find('?');
    if (queryPos != std::string::npos)
    {
        newPath = targetPath.substr(0, queryPos); 
    }
    if (findLocation)
    {
        struct stat s;
        if (stat(newPath.c_str(), &s) == 0)
        {
            if (s.st_mode & S_IFDIR)
            {
                if(newPath[newPath.size()-1]!='/')
                    newPath +="/";
                newPath += findLocation->getIndex();
            }
        }
        if (!findLocation->getCgiExtension().empty() && !findLocation->getCgiPass().empty())
        {
            if (client[clientSocket]->method.isCGIPath(newPath, *findLocation))
            {
                client[clientSocket]->cgi.cgi_is_complet = false;
                if (!client[clientSocket]->method.handleCGI(request, newPath, clientSocket, *findLocation, server,client[clientSocket]->cgi))
                { 
                    HttpResponse::sendError(clientSocket, 500, "Internal Server Error", server);
                    client[clientSocket]->cgi.cgi_is_complet = true;
                    return;
                }
                return;
            }
        }
    }
    if (request.getMethod() == "GET")
    {
        client[clientSocket]->method.response_complet = false;
        client[clientSocket]->method.HandleMethodGet(request, clientSocket, findLocation, targetPath, server);
    }
    else if (request.getMethod() == "POST")
    {
       client[clientSocket]->method.HandleMethodPost(client[clientSocket]->request, clientSocket, *findLocation, targetPath, server);
    }
    else if (request.getMethod() == "DELETE")
    {
       client[clientSocket]->method.HandleMethodDelete(request, clientSocket, *findLocation, targetPath, server);
    }
    else
    {
        HttpResponse::sendError(clientSocket, 405, "Method Not Allowed", server);
    }
}
void Server::_checkTimeouts(time_t currentTime, std::vector<ServerConfig> &servers, std::map<int, s_sockaddr> server_map)
{
    std::map<int, Client*>::iterator it = this->client.begin();

    while (it != this->client.end())
    {
        if (currentTime - it->second->start_time_client >= INACTIVITY_TIMEOUT)
        {
            if (it->second->cgi.stillRunning)
            {
                // std::cout << "Process is still running, terminating..." << std::endl;
                it->second->cgi.killChild(); 
            }

            HttpResponse::sendError(it->first, 408, "Request Timeout", servers[server_map[it->first].nbr_server]);
            // std::cerr << "[Server::_checkTimeouts] Client " << it->first << " timed out" << std::endl;

            int client_fd = it->first;
            int epoll_fd = it->second->fd;
            std::string pathfile = it->second->request.get_pathfile(); 

            std::map<int, Client*>::iterator tmp = it++;
            delete tmp->second;
            this->client.erase(tmp);

            std::remove(pathfile.c_str());

            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
            std::cout<<"cline close."<<std::endl;
            close(client_fd);
        }
        else
        {
            ++it;
        }
    }
}

void Server::start_server(std::vector<ServerConfig> &servers)
{
    std::map<int, s_sockaddr> server_map;

    server_map = create_server(servers);
    struct epoll_event ev, events[MAX_EVENTS];
    int epollfd = epoll_create(MAX_EVENTS);
    if (epollfd == -1)
    {
        std::cerr << "Epoll creation failed: " << std::endl;
        return;
    }
    std::map<int, s_sockaddr>::iterator entry = server_map.begin();
    for (; entry != server_map.end(); ++entry)
    {
        ev.events = EPOLLIN ;
        ev.data.fd = entry->first;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, entry->first, &ev) == -1)
        {
            std::cerr << "Epoll add failed for socket: " << entry->first << std::endl;
            close(entry->first);
            continue;
        }
    }
    time_t lastTimeoutCheck = time(NULL);
    int serv_nbr = -1;
    while (true)    
    {
        int nfds = epoll_wait(epollfd, events, MAX_EVENTS, 4000);
        if (nfds == -1)
        {
            continue;
        }
        for (int n = 0; n < nfds; ++n)
        {
            int event_fd = events[n].data.fd;
            if (server_map.find(event_fd) != server_map.end())
            {
                accept_new_connection_request(epollfd, event_fd, ev);
                serv_nbr = event_fd;
            }
            
            else if(client.find(event_fd)==client.end())
                continue;
            else
            {
                if ((events[n].events & EPOLLERR) || (events[n].events & EPOLLRDHUP) || (events[n].events & EPOLLHUP))
                {
                    if(client[event_fd]->method.response_complet&&client[event_fd]->cgi.cgi_is_complet)
                    {
                        std::remove(client[event_fd]->request.get_pathfile().c_str());
                        delete client[ev.data.fd];
                        client.erase(ev.data.fd);
                        epoll_ctl(epollfd, EPOLL_CTL_DEL, ev.data.fd, NULL);
                        close(ev.data.fd);
                    }
                }
                if((events[n].events & EPOLLIN)&&!client[event_fd]->request.requestcomplet)
                {
                    client[event_fd]->start_time_client = time(NULL);
                    client[event_fd]->return_handelclieant = handleClient(event_fd);
                }
                else if (events[n].events & EPOLLOUT&&client[event_fd]->request.requestcomplet)
                {
                    // client[event_fd]->start_time_client = time(NULL);
                    // std::cout<<"CLIENT FD: "<<event_fd<<"modif time."<<std::endl;
                    if(!client[event_fd]->return_handelclieant&&client[event_fd]->request.requestcomplet)
                    {
                        HttpResponse::sendError(event_fd, 400, "Bad Request", servers[server_map[serv_nbr].nbr_server]);
                        std::remove(client[event_fd]->request.get_pathfile().c_str());
                        delete client[event_fd];
                        client.erase(event_fd);
                        epoll_ctl(epollfd, EPOLL_CTL_DEL, event_fd, NULL);
                        close(event_fd);
                    }
                    else
                    {
                        ServerConfig serv = servers[server_map[serv_nbr].nbr_server];
                        ServerConfig server = servers[server_map[serv_nbr].nbr_server];
                        std::string Host_name = client[event_fd]->request.getHeader("Host");
                        if(!Host_name.empty())
                        {
                            size_t end = Host_name.find_last_not_of(" \t\r\n");
                            if (end != std::string::npos)
                                Host_name.erase(end + 1);
        
                            size_t start = Host_name.find_first_not_of(" \t\r\n");
                            if (start != std::string::npos)
                                Host_name.erase(0, start);
                            std::stringstream ss;
                            ss << server_map[serv_nbr].port;
                            std::string Host_server = server_map[serv_nbr].host + ":" + ss.str();
        
                            if (Host_server != Host_name) {
                                for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); ++it) {
                                    if (it->getServerName().find(Host_name) != std::string::npos) {
                                        server = *it;
                                        break;
                                    }
                                }
                            }
                        }
                        if (client[event_fd]->request.requestcomplet)
                        {
                            size_t maxBodySize = server.getClientMaxBodySize();
                            if (client[event_fd]->request.getcontentLength() > maxBodySize)
                            {
                                HttpResponse::sendError(event_fd, 413, "Request Entity Too Large", server);
                            }
                            else if(client[event_fd]->request.requestcomplet)
                                handleMethod(event_fd, server);
                            if(client[event_fd]->method.response_complet&&client[event_fd]->cgi.cgi_is_complet)
                            {
                                std::cout<<"close client "<<std::endl;
                                std::remove(client[event_fd]->request.get_pathfile().c_str());
                                delete client[event_fd];
                                client.erase(event_fd);
                                epoll_ctl(epollfd, EPOLL_CTL_DEL, event_fd, NULL);
                                close(event_fd);
                            }
                        }
                    }

                }
            }
        }
        time_t currentTime = time(NULL);
		if (currentTime - lastTimeoutCheck >= TIMEOUT_CHECK_INTERVAL)
        {
            this->_checkTimeouts(currentTime,servers,server_map);
			lastTimeoutCheck = currentTime;
        }
    }
    close(epollfd);
    std::map<int, s_sockaddr>::iterator it = server_map.begin();
    for (; it != server_map.end(); it++)
        close(it->first);
}

Server::~Server() {
    for (std::map<int, Client*>::iterator it = client.begin(); it != client.end(); ++it) {
        close(it->first);
        delete it->second;
    }
    client.clear();
}

