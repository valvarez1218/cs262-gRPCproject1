#include <string>
#include <iostream>
// #include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>

using grpc::Channel;

#define PORT 8080

bool establishConnection() {
    while (true) {
        std::string ip_addr;
        std::cout << "Input IP Address of Server: ";
        std::cin >> ip_addr;
        grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    }

    return true;
}