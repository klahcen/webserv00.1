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
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
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

    while (true)
    {
        char mesg[1024];
        memset(mesg, 0, 1024);
        std::cout<<"INPUT A MESSAGE TO SERVER !Q TO QUIT: ";
        std::cin>>mesg;
   
        if(mesg[0]=='Q')
            break;
        send(clientsocket_fd, mesg, strlen(mesg),0);
        memset(mesg, 0, 1024);
        recv(clientsocket_fd, mesg, 1024,0);
        // write(clientsocket_fd, mesg, strlen(mesg));
        // read(clientsocket_fd, mesg, 1024);
        std::cout<<"recieved message freom server: "<<mesg<<std::endl;
    }
    std::cout<<"Cline is disconnectd."<<std::endl;
    close(clientsocket_fd);
    return 0;
}


// #include <iostream>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <unistd.h>
// #include <cstring>
// #include <arpa/inet.h>

// #define PORT 8080
// #define SERVER_IP "127.0.0.1"  // Change this to the actual server IP if needed

// int main() {
//     // Create a socket
//     int client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (client_socket_fd < 0) {
//         std::cerr << "Socket creation failed" << std::endl;
//         return -1;
//     }

//     // Define the server address
//     struct sockaddr_in server_addr;
//     memset(&server_addr, 0, sizeof(server_addr));
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(PORT);

//     // Convert the IP address from text to binary form
//     if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
//         std::cerr << "Invalid address or Address not supported" << std::endl;
//         close(client_socket_fd);
//         return -1;
//     }

//     // Attempt to connect to the server
//     if (connect(client_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0) {
//         std::cout << "Connected to the server." << std::endl;
//     } else {
//         std::cerr << "Connection failed" << std::endl;
//         close(client_socket_fd);
//         return -1;
//     }

//     while (true) {
//         std::string message;
//         std::cout << "INPUT A MESSAGE TO SERVER (!Q to quit): ";
//         std::getline(std::cin, message);

//         // If the message is "!Q", break and disconnect
//         if (message == "!Q") {
//             break;
//         }

//         // Send message to the server
//         if (send(client_socket_fd, message.c_str(), message.size(), 0) < 0) {
//             std::cerr << "Send failed" << std::endl;
//             break;
//         }

//         // Prepare to receive the server's response
//         char buffer[1024] = {0};
//         int bytes_received = recv(client_socket_fd, buffer, 1024, 0);
//         if (bytes_received <= 0) {
//             std::cerr << "Server closed the connection or error occurred" << std::endl;
//             break;
//         }

//         // Print the received message from the server
//         std::cout << "Received message from server: " << buffer << std::endl;
//     }

//     std::cout << "Client is disconnected." << std::endl;
//     close(client_socket_fd);
//     return 0;
// }