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

int main()
{
    int clientsocket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientsocket_fd == 0)
    {
        std::cerr << "Socket failed" << std::endl;
        return -1;
    }
    std::string ip="127.54.54.03";
    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(port);
    client_addr.sin_addr.s_addr = INADDR_ANY;
    size_t socksize = sizeof(struct sockaddr_in);

    if(connect(clientsocket_fd, (struct sockaddr*)&client_addr, socksize)==0)
        std::cout<<"connect "<<std::endl;
    else
    {
        std::cerr << "connect failed" << std::endl;
        close(clientsocket_fd);
        return -1;
    }
        char buffer[1024];

    while (true)
    {
        ssize_t  amountReceived = recv(clientsocket_fd,buffer,1024,0);

        if(amountReceived>0)
        {
            buffer[amountReceived] = 0;
            printf("%s\n",buffer);
            send(clientsocket_fd,buffer, strlen(buffer),0);
        }

        // if(amountReceived==0)
        //     break;
    }
    close(clientsocket_fd);
    return 0;
}