#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <poll.h>
#include <vector>

#define PORT "6667"
#define MAX_CLIENTS 10
#define MAX_MESSAGE_LENGTH 512

int main() {
    int server_socket, client_socket;
    struct addrinfo hints, *server_info, *p;
    struct sockaddr_storage client_addr;
    socklen_t client_addr_len;
    struct pollfd poll_fds[MAX_CLIENTS + 1]; // +1 for server socket
    char message_buffer[MAX_MESSAGE_LENGTH];
    vector<int> clients; // vector to store client sockets <=> fd

    // get address information for server
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, PORT, &hints, &server_info);	//	freeaddrinfo missing

    // create server socket
    server_socket = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

    // set socket options to allow reuse of address
    int yes = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

    // bind socket to address and port
    bind(server_socket, server_info->ai_addr, server_info->ai_addrlen);

    // listen for incoming connections
    listen(server_socket, MAX_CLIENTS);

    // add server socket to poll file descriptors
    poll_fds[0].fd = server_socket;
    poll_fds[0].events = POLLIN;

    while (true) {
        // poll for events on all file descriptors
        poll(poll_fds, clients.size() + 1, -1);

        // check for incoming connections on server socket
        if (poll_fds[0].revents & POLLIN) {
            client_addr_len = sizeof client_addr;
            client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);

            // add client socket to vector and poll file descriptors
            clients.push_back(client_socket);
            poll_fds[clients.size()].fd = client_socket;
            poll_fds[clients.size()].events = POLLIN;

            std::cout << "New client connected\n";
        }

        // check for incoming messages from clients
        for (size_t i = 0; i < clients.size(); i++) {
            if (poll_fds[i+1].revents & POLLIN) {
                int bytes_received = recv(clients[i], message_buffer, MAX_MESSAGE_LENGTH, 0);
                if (bytes_received <= 0) {
                    // client disconnected, remove from vector and poll file descriptors
                    close(clients[i]);
                    clients.erase(clients.begin() + i);
                    poll_fds[i+1] = poll_fds[clients.size() + 1];
                    clients.pop_back();
                } else {
                    // echo message back to client
                    for (;;){
                        if (send(clients[i], message_buffer, bytes_received, 0) == -1)
                            break ;
                    }
                }
            }
        }
    }

    return 0;
}
