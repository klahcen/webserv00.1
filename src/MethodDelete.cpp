#include "../include/Method.hpp"
#include "../include/CGIHandler.hpp"
#include "../include/HttpRequest.hpp"
#include "../include/HttpResponse.hpp"
#include <algorithm>

std::vector<std::string> getDirectoryListing(std::string &path)
{
    DIR *dir = opendir(path.c_str());
    struct dirent *entry;
    std::vector<std::string> content;

    if (dir)
    {
        path = "";
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_name[0] != '.')
            {
                content.push_back(entry->d_name); // Use push_back to avoid out-of-bounds access
            }
        }
        closedir(dir);
    }
    return content;
}

bool isDir(std::string path)
{
    struct stat s;

    if (stat(path.c_str(), &s) != 0)
        return 0;
    return S_ISDIR(s.st_mode);
}

bool Method::RemoveContentDir(std::string str, int clientSocket)
{

    struct dirent *dr;
    std::string response;
    DIR *dir = opendir(str.c_str());
    if (!dir)
    {
        response = buildResponse(403, "Forbidden", "Forbidden.");
        if(send(clientSocket, response.c_str(), response.size(), 0)<=0)
        { 
            std::cerr<<"Write failed: "<<std::endl;
        }
        return false;
    }
    while ((dr = readdir(dir)))
    {
        std::string name = dr->d_name;
        if (name != "." && name != "..")
        {
            if (isDir(str + name) == true)
                RemoveContentDir(str + name, clientSocket);
            else
            {
                if (std::remove((str + name).c_str()) == -1)
                {
                    closedir(dir);
                    response = buildResponse(500, "Internal Server Error", "Internal Server Error.");
                    if(send(clientSocket, response.c_str(), response.size(), 0)<=0)
                    { 
                        std::cerr<<"Write failed: "<<std::endl;
                    }
                    return false;
                }
            }
        }
    }
    if (std::remove((str).c_str()) == -1)
    {
        closedir(dir);
        response = buildResponse(500, "Internal Server Error", "Internal Server Error.");
        if(send(clientSocket, response.c_str(), response.size(), 0)<=0)
        { 
            std::cerr<<"Write failed: "<<std::endl;
        }
        return false;
    }
    closedir(dir);
    return true;
}

void Method::HandleMethodDelete(HttpRequest& request, int clientSocket, const RouteConfig &route, std::string fullPath, ServerConfig server)
{
    (void)request;
    (void)route;
    (void)server;
    std::string check;
    std::string response;
    struct stat s;

    check = fullPath;
    size_t queryPos = check.find('?');
    if (queryPos != std::string::npos)
    {
        check = fullPath.substr(0, queryPos); // Remove query string from the path
    }
    if (stat(check.c_str(), &s) == 0)
    {
        if (s.st_mode & S_IFDIR) // It's a directory
        {
            // If it's a directory and doesn't end with '/', return Conflict (409)
            if (check[check.size() - 1] != '/')
            {
                response = buildResponse(409, "Conflict", "Conflict with existing data.");
                if(send(clientSocket, response.c_str(), response.size(), 0)<=0)
                    std::cerr<<"Write failed: "<<std::endl;
                return;
            }
            else
            {
                if (!RemoveContentDir(check, clientSocket))
                    return;
                response = buildResponse(200, "OK", "Resource deleted.");
                if(send(clientSocket, response.c_str(), response.size(), 0)<=0)
                { 
                    std::cerr<<"Write failed: "<<std::endl;
                }
            }
        }
        else if (s.st_mode & S_IFREG) // It's a regular file
        {
            // Ensure the file exists and is writable
            if (access(fullPath.c_str(), F_OK) == -1 || access(fullPath.c_str(), W_OK) == -1)
            {
                response = buildResponse(404, "Not Found", "Resource not found.");
                if(send(clientSocket, response.c_str(), response.size(), 0)<=0){ 
                    std::cerr<<"Write failed: "<<std::endl;
                }
                return;
            }

            // Attempt to remove the file
            if (std::remove(fullPath.c_str()) != 0)
            {
                std::cerr << "DELETE -> remove error" << std::endl;
                response = buildResponse(404, "Not Found", "Resource not found.");
                if(send(clientSocket, response.c_str(), response.size(), 0)<=0)
                { 
                    std::cerr<<"Write failed: "<<std::endl;
                }
                return;
            }

            // If successful, you may want to return a success message
            response = buildResponse(204, "No Content", "Resource deleted.");
            if(send(clientSocket, response.c_str(), response.size(), 0)<=0)
            { 
                std::cerr<<"Write failed: "<<std::endl;
            }
        }
    }
    else
    {
        // If stat fails (file doesn't exist)
        response = buildResponse(404, "Not Found", "Resource not found.");
        if(send(clientSocket, response.c_str(), response.size(), 0)<=0)
        { 
            std::cerr<<"Write failed: "<<std::endl;
        }
    }
}