#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#define port 8080

int main()
{
    //creat socket
    int socket_fd = socket(AF_INET, SOCK_STREAM,0);
    if(socket_fd==0)
    {
        std::cerr << "Socket failed" << std::endl;
        return -1;
    }
    //Bind the Socket to a Port
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_NONE;

    if(bind(socket_fd, (const struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr<<"bind failed"<<std::endl;
        close(socket_fd);
        return -1;
    }
    //
}