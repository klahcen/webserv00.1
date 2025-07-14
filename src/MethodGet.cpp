#include "../include/Method.hpp"
#include "../include/CGIHandler.hpp"
#include "../include/HttpResponse.hpp"
#include <cstddef>
#include <cstring>
#include <iostream>
#include <map>
#include <algorithm>
#include <sstream>
#include <string>
#include <sys/types.h>

Method::Method()
{
    totalSent=0;
    htmlContent = "";
    first_time=false;
    response ="";
    response_complet = false;
    total_Re = 0;
}

bool Method::readFile(const std::string &filename)
{
    std::ifstream file(filename.c_str());
    if (file.fail())
    {
        return false;
    }
    return true;
}

std::map<std::string, std::string> parseQueryStringGet(const std::string &query)
{
    std::map<std::string, std::string> queryParams;
    std::istringstream queryStream(query);
    std::string keyValuePair;

    while (std::getline(queryStream, keyValuePair, '&'))
    {
        size_t equalPos = keyValuePair.find('=');
        if (equalPos != std::string::npos)
        {
            std::string key = keyValuePair.substr(0, equalPos);
            std::string value = keyValuePair.substr(equalPos + 1);
            queryParams[key] = value;
        }
        else
        {
            queryParams[keyValuePair] = ""; // Handle keys with no value
        }
    }

    return queryParams;
}

void Method::send_request(HttpRequest request, const RouteConfig *route, std::string fullpath, std::string path, int clientSocket, int status_code, ServerConfig server)
{
    (void)fullpath;
    (void)server;
    (void)request;
    (void)route;
    std::string newPath = path;
    size_t queryPos = newPath.find('?');
    if (queryPos != std::string::npos)
    {
        newPath = path.substr(0, queryPos); // Remove query string from the path
    }
    if(!first_time)
    {
        std::string cleanPath = path;
        size_t position = cleanPath.find('?');
        if (position != std::string::npos)
        {
            cleanPath = path.substr(0, position); // Remove query string from the path
        }
        path = cleanPath;
        if (!readFile(path))
        {   
            response_complet = true;
            HttpResponse::sendError(clientSocket, 404, "Not Found", server);
            return;
        }
        if (path.find('.') == std::string::npos)
            path += '.';
        std::stringstream oss;
        oss << status_code; // Insert the integer into the stream
        std::string numberStr = oss.str();
        oss.str("");
        oss.clear();
        file_get.open(path.c_str(),std::ios::in | std::ios::binary);
        file_get.seekg(0, std::ios::end);
        total_Re = file_get.tellg();
        file_get.seekg(0, std::ios::beg);
        oss << total_Re;
        std::string size_body = oss.str();
        response =
        "HTTP/1.1 " + numberStr + " OK\r\n"
        "Content-Type: " +
        getMimeType(std::strrchr(path.c_str(), '.')) + "\r\n"
        "Content-Length: " +
        size_body + "\r\n"
        "Connection: close\r\n\r\n";
        first_time = true;
        total_Re+=(ssize_t)response.size();
        int i = send(clientSocket, response.c_str(), response.size(), 0);
        if(i<=0)
        { 
            std::cerr<<"Write failed: "<<std::endl;
        }
        else
            totalSent+=i;
    }
    std::getline(file_get, response);
    response+='\n';
    int i = send(clientSocket, response.c_str(), response.size(), 0);
    if (i <= 0) return;
    totalSent += i;
    if (totalSent >= total_Re)
        response_complet = true;
}

std::string get_directory(std::string path)
{
    std::string str;
    if (path == "/")
        return path;
    if (path[0] == '/')
        str = path.substr(path.find_first_not_of('/'));
    str = str.substr(0, str.find_last_of('/'));
    return str;
}

std::string generateDirectoryListing(std::string &path)
{
    DIR *dir = opendir(path.c_str());
    struct dirent *entry;
    std::string content = "<!DOCTYPE html> \
                            <html> \
                            <head><title>Index of " +
                          path + "</title></head> \
                            <body> \
                            <h1>Index of " +
                          path + "</h1> \
                            <ul>";

    if (dir)
    {
        path = "";
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_name[0] != '.')
            {
                content += "<li><a href=\"" + path + entry->d_name + "\">" + entry->d_name + "</a></li>";
            }
        }
        closedir(dir);
    }

    content += "</ul></body></html>";
    return content;
}

void Method::sendDirectoryListingResponse(int clientSocket, std::string &path)
{
    std::stringstream oss;
    std::string content = generateDirectoryListing(path);
    oss << content.size();
    std::string numberStr = oss.str();
    std::string response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html\r\n"
                           "Content-Length: " +
                           numberStr + "\r\n"
                                       "Connection: close\r\n"
                                       "\r\n" +
                           content;

    if(send(clientSocket, response.c_str(), response.size(), 0)<=0)
    { 
        std::cerr<<"Write failed: "<<std::endl;
    }
}

void Method::sendRedirectResponse(int clientSocket, const std::string &path)
{
    std::string newPath = path;
    std::string response = "HTTP/1.1 301 Moved Permanently\r\n"
                           "Location: " +
                           newPath + "/\r\n"
                                     "Connection: close\r\n"
                                     "\r\n";

    if(send(clientSocket, response.c_str(), response.size(), 0)<=0)
    { 
        std::cerr<<"Write failed: "<<std::endl;
    }
}

void Method::send_request_auto(HttpRequest request, int clientSocket, const RouteConfig *route, std::string fuh, ServerConfig server)
{
    struct stat s;
    std::string root;
    std::string index1 = "";
    std::string path;
    std::string check;
    if (route)
    {
        path = route->getPath();
        index1 = route->getIndex();
    }
    else
    {
        index1 = server.getIndex();
    }

    root = fuh;
    check = fuh;
    std::string::size_type pos = fuh.find('?');
    std::string checkPath = fuh;
    if (pos != std::string::npos)
        checkPath = fuh.substr(0, pos);
    check = checkPath;
    if (stat(check.c_str(), &s) == 0)
    {
        if (s.st_mode & S_IFDIR)
        {
            if (check[check.size() - 1] != '/')
                sendRedirectResponse(clientSocket, request.getPath());
            if (!index1.empty())
            {
                size_t queryPos = root.find('?');
                if (queryPos != std::string::npos)
                {
                    root = root.substr(0, queryPos);
                }
                send_request(request, route, fuh, root + index1, clientSocket, 200, server);
            }
            else
            {
                std::string autoindex;
                if (route)
                    autoindex = (route->getAutoIndex() ? "on" : "off");
                else
                autoindex = (server.get_Autoindex() ? "on" : "off");
                if (autoindex == "off")
                {
                    HttpResponse::sendError(clientSocket, 403, "Forbidden", server);
                    return;
                }
                else
                    sendDirectoryListingResponse(clientSocket, fuh);
            }
        }
        else if (s.st_mode & S_IFREG)
            send_request(request, route, fuh, root, clientSocket, 200, server);
        else
        {
            HttpResponse::sendError(clientSocket, 403, "Forbidden", server);
        }
    }
    else
        HttpResponse::sendError(clientSocket, 403, "Page Not Found", server);
}

std::string strtrim(const std::string &str)
{
    size_t start = str.find_first_not_of("/");
    if (start == std::string::npos)
        return "";
    size_t end = str.find_last_not_of("/");
    return str.substr(start, end - start + 1);
}

void Method::HandleMethodGet(HttpRequest request, int clientSocket, const RouteConfig *route, std::string fullPath, ServerConfig server)
{
    std::string root;
    std::string index1 = "";
    std::string path = request.getPath();
    send_request_auto(request, clientSocket, route, fullPath, server);
}

void Method::BadMethod(std::vector<RouteConfig>::iterator route, int clientSocket)
{
    std::string response;
    if (!allowd_method(route->getAllowedMethods(), "POST"))
    {
        response = buildResponse(405, "Method Not Allowed", "POST method is not allowed.");
        if(send(clientSocket, response.c_str(), response.size(), 0)<=0)
        { 
            std::cerr<<"Write failed: "<<std::endl;
        }
        return;
    }
}
