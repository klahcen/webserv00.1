#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <arpa/inet.h>
#define port 8080

std::string readFile(const std::string &filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
    {
        return "";
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

int main()
{
    // creat socket
    int serversocket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (serversocket_fd == 0)
    {
        std::cerr << "Socket failed" << std::endl;
        return -1;
    }
    std::string ip="172.54.54.03";
    // Bind the Socket to a Port
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    // inet_pton(AF_INET, ip.c_str(),&address.sin_addr);
    address.sin_addr.s_addr = INADDR_ANY;
    size_t socksize = sizeof(struct sockaddr_in);

    if (bind(serversocket_fd, (const struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "bind failed" << std::endl;
        close(serversocket_fd);
        return -1;
    }
    // Listen for Incoming Connections
    if (listen(serversocket_fd, 3) < 0)
    {
        std::cerr << "listen failed" << std::endl;
        close(serversocket_fd);
        return -1;
    }
    std::cout << "Server is listening on port " << port << std::endl;
    for (;;)
    {
        // Accept a Connection
        ssize_t new_socket = accept(serversocket_fd, (struct sockaddr *)&address, (socklen_t *)&socksize);
        if (new_socket < 0)
        {
            std::cerr << "accept failed" << std::endl;
            close(serversocket_fd);
            return -1;
        }

        // Read the HTML file
        std::string htmlContent = readFile("../page_web/page.html");
        if (htmlContent.empty())
        {
            const char *notFoundResponse =
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 9\r\n"
                "Connection: close\r\n"
                "\r\n"
                "Not Found";
            send(new_socket, notFoundResponse, strlen(notFoundResponse), 0);
        }
        else
        {
            std::stringstream ss;
            ss << htmlContent.size();
            // Prepare the response
            std::string response =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: " + ss.str() + "\r\n"
                "Connection: close\r\n"
                "\r\n" +
                htmlContent;
            // Send the response
            send(new_socket, response.c_str(), response.size(), 0);
        }
        close(new_socket);
    }
    close(serversocket_fd);
    return 0;
}