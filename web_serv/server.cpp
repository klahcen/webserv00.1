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
#include <sys/time.h>
#include <sys/types.h>
#define port 8080

int creat_socket(void)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == 0)
    {
        std::cerr << "Socket failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    return fd;
}

struct sockaddr_in init_sockaddr(void)
{
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;
    return address;
}

void bind_socket(int serversocket_fd, struct sockaddr_in address)
{
    if (bind(serversocket_fd, (const struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "bind failed" << std::endl;
        close(serversocket_fd);
        exit(EXIT_FAILURE);
    }
}


void list_of_socket(int serversocket_fd)
{
    if (listen(serversocket_fd, 3) < 0)
    {
        std::cerr << "listen failed" << std::endl;
        close(serversocket_fd);
        exit(EXIT_FAILURE);
    }
}

int main()
{
    int serversocket_fd = creat_socket();
    struct sockaddr_in address = init_sockaddr();
    size_t socksize = sizeof(struct sockaddr_in);
    bind_socket(serversocket_fd, address);
    list_of_socket(serversocket_fd);
    int setect_fd;
    fd_set FDsCopy;
    fd_set FDs;
    struct timeval timeout;

    FD_ZERO(&FDs);
    FD_SET(serversocket_fd, &FDs);
    int fdMax = serversocket_fd;
    int fdNum;

    char buffet[1024];

    int connectionServerSockFD;
    struct sockaddr_in connectClinetAddress;
    socklen_t clintAddrLengh = sizeof(connectClinetAddress);
    memset(&connectClinetAddress, 0, clintAddrLengh);
    std::cout << "Server is listening on port " << port << std::endl;
    for (;;)
    {
        FDsCopy = FDs;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        fdNum = select(serversocket_fd+1, &FDsCopy, 0,0, &timeout);

        if(fdNum == -1)
        {
            std::cout<<"select failed"<<std::endl;
            exit (EXIT_FAILURE);
        }
        else if(fdNum == 0)
            continue;
        else
        {
            int i;
            for(i = 3; i < (fdMax+1);i++)
            {
                if(FD_ISSET(i, &FDsCopy))
                {
                    if(i==serversocket_fd)
                    {
                        //accept  the connection request
                        connectionServerSockFD = accept(serversocket_fd, (struct sockaddr *)&connectClinetAddress, (socklen_t *)&clintAddrLengh);
                        FD_SET(connectionServerSockFD, &FDs);
                         if (connectionServerSockFD == -1)
                        {
                            std::cerr << "accept failed" << std::endl;
                            continue;
                        }
                        else if (fdMax < connectionServerSockFD)
                            fdMax = connectionServerSockFD;
                        std::cout<<"Client with ID: "<<connectionServerSockFD<<" is connected."<<std::endl;
                    }
                    else//read a message
                    {
                        memset(buffet, 0, 1024);
                        int stringLength = read(i, buffet, 1024);
                        if(stringLength == 0)
                        {
                            FD_CLR(i, &FDs);
                            close(i);
                            std::cout<<"Client With ID: "<<i<<" is diseconnected"<<std::endl;
                        }
                        else//echa response
                            write(i, buffet, stringLength);
                    }
                }
            }
        }
    }
    close(serversocket_fd);
    return 0;
}



// #include <iostream>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <unistd.h>
// #include <cstring>
// #include <arpa/inet.h>
// #include <sys/time.h>
// #include <sys/types.h>

// #define PORT 8080

// int create_socket(void) {
//     int fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (fd < 0) {
//         std::cerr << "Socket creation failed" << std::endl;
//         exit(EXIT_FAILURE);
//     }
//     return fd;
// }

// struct sockaddr_in init_sockaddr(void) {
//     struct sockaddr_in address;
//     address.sin_family = AF_INET;
//     address.sin_port = htons(PORT);
//     address.sin_addr.s_addr = INADDR_ANY;
//     return address;
// }

// void bind_socket(int server_socket_fd, struct sockaddr_in address) {
//     if (bind(server_socket_fd, (const struct sockaddr *)&address, sizeof(address)) < 0) {
//         std::cerr << "Bind failed" << std::endl;
//         close(server_socket_fd);
//         exit(EXIT_FAILURE);
//     }
// }

// void listen_on_socket(int server_socket_fd) {
//     if (listen(server_socket_fd, 3) < 0) {
//         std::cerr << "Listen failed" << std::endl;
//         close(server_socket_fd);
//         exit(EXIT_FAILURE);
//     }
// }

// int main() {
//     int server_socket_fd = create_socket();
//     struct sockaddr_in address = init_sockaddr();
//     size_t addrlen = sizeof(struct sockaddr_in);
//     bind_socket(server_socket_fd, address);
//     listen_on_socket(server_socket_fd);

//     fd_set master_fds;   // master file descriptor set
//     fd_set read_fds;     // temp file descriptor set for select()
//     struct timeval timeout;

//     FD_ZERO(&master_fds); // clear the master and temp sets
//     FD_ZERO(&read_fds);

//     FD_SET(server_socket_fd, &master_fds); // add the server socket to the master set
//     int fd_max = server_socket_fd;         // keep track of the biggest file descriptor

//     char buffer[1024];
//     struct sockaddr_in client_address;
//     socklen_t client_addr_len = sizeof(client_address);
//     std::cout << "Server is listening on port " << PORT << std::endl;

//     for (;;) {
//         read_fds = master_fds; // copy the master set into the temp set
//         timeout.tv_sec = 5;
//         timeout.tv_usec = 0;

//         int activity = select(fd_max + 1, &read_fds, nullptr, nullptr, &timeout);

//         if (activity < 0 && errno != EINTR) {
//             std::cerr << "Select error" << std::endl;
//             break;
//         }

//         // Check if there's a new connection on the server socket
//         if (FD_ISSET(server_socket_fd, &read_fds)) {
//             int new_socket = accept(server_socket_fd, (struct sockaddr *)&client_address, &client_addr_len);
//             if (new_socket < 0) {
//                 std::cerr << "Accept failed" << std::endl;
//                 continue;
//             }

//             // Add the new socket to the master set
//             FD_SET(new_socket, &master_fds);
//             if (new_socket > fd_max) {
//                 fd_max = new_socket;
//             }
//             std::cout << "Client with ID: " << new_socket << " connected." << std::endl;
//         }

//         // Check all clients for incoming data
//         for (int i = 0; i <= fd_max; i++) {
//             if (i != server_socket_fd && FD_ISSET(i, &read_fds)) {
//                 memset(buffer, 0, sizeof(buffer));
//                 int bytes_read = read(i, buffer, sizeof(buffer));

//                 if (bytes_read <= 0) {
//                     // If the client disconnected, or an error occurred
//                     if (bytes_read == 0) {
//                         std::cout << "Client with ID: " << i << " disconnected." << std::endl;
//                     } else {
//                         std::cerr << "Read error from client with ID: " << i << std::endl;
//                     }
//                     close(i);
//                     FD_CLR(i, &master_fds);
//                 } else {
//                     // Echo the received message back to the client
//                     write(i, buffer, bytes_read);
//                 }
//             }
//         }
//     }

//     close(server_socket_fd);
//     return 0;
// }