#ifndef METHOD_HPP
#define METHOD_HPP

#include "HttpRequest.hpp"
#include <cstdio>
#include <fstream>
#include <sys/stat.h>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include "RouteConfig.hpp"
#include "ServerConfig.hpp"
#include <ctime>
#include <sys/socket.h>
#include "CGIHandler.hpp"

class Method
{
private:
    std::string generaterandomfile();
    bool readFile(const std::string &filename);
    void send_request_auto(HttpRequest request, int clientSocket, const RouteConfig *route, std::string fuh, ServerConfig server);
    void sendFileOverSocket(int clientSocket, const std::string& response, const std::string& ppath);
    ssize_t totalSent;
    std::string htmlContent;
    bool first_time;
    std::string response;
    ssize_t total_Re;
    std::ifstream file_get;

public:
    Method();
    bool response_complet;
    bool RemoveContentDir(std::string str,int clientSocket);
    void sendRedirectResponse(int clientSocket, const std::string &path);
    bool next_check_dir(HttpRequest& request, int clientSocket, const RouteConfig &route, std::string fullPath, ServerConfig server);
    std::string getMimeType(const std::string extension);
    bool allowd_method(const std::vector<std::string> &vector, const std::string &method);
    void HandleMethodGet(HttpRequest request, int clientSocket, const RouteConfig *route, std::string fullPath, ServerConfig server);
    void HandleMethodPost(HttpRequest& request, int clientSocket, const RouteConfig &route, std::string fullPath, ServerConfig server);
    void HandleMethodDelete(HttpRequest& request, int clientSocket, const RouteConfig &route, std::string fullPath, ServerConfig server);
    std::string buildResponse(int statusCode, const std::string &statusMessage, const std::string &body);
    void BadMethod(std::vector<RouteConfig>::iterator route, int clientSocket);
    void send_request(HttpRequest request, const RouteConfig *route, std::string fullpath, std::string path, int clientSocket, int status_code, ServerConfig server);
    void sendDirectoryListingResponse(int clientSocket, std::string &path);

    /// cgi
    bool handleCGI(HttpRequest &request, const std::string &fullPath, int clientSocket, const RouteConfig &route, ServerConfig server,CGIHandler &cgiHandler);
    std::string getFileExtension(const std::string &filePath);
    bool isInterpreterCompatible(const std::string &interpreter, const std::string &fileExtension);
    std::map<std::string, std::string> extractQueryParams(HttpRequest &request);
    void parseQueryString(const std::string &queryString, std::map<std::string, std::string> &queryParams);
    std::string decodeURIComponent(const std::string &encoded);
    bool isCGIPath(const std::string &fullPath, const RouteConfig &route);
};

#endif
