#include <iostream>
#include <sys/socket.h>

#define port 8080

int main()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM,0);
    if(socket_fd==0)
    {
        std::cerr << "Socket failed" << std::endl;
        return -1;
    }


}