#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <iostream>

#define PORT 8080

int server_socket = 0;
int client_fd;

bool establishConnection() {
    sockaddr_in serv_addr;
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket Creation Error \n");
        return false;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    bool connected_to_server = false;

    while (!connected_to_server) {
        std::string ip_addr;
        std::cout << "Input IP Address of Server: ";
        std::cin >> ip_addr;
        if (inet_pton(AF_INET, ip_addr.c_str(), &serv_addr.sin_addr) <= 0) {
            printf("\nInvalid address/ Address not supported \n");
            continue;
        }

        if ((client_fd = connect(server_socket, (sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
            printf("\n Connection Failed \n");
            continue;
        }

        connected_to_server = true;
    }

    return true;
}