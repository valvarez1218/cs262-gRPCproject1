// #include "../chatService.pb.h"
#include "serviceImplementations.h"
// #include "clientHandler.h"

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <netdb.h>
// #include <netinet/in.h>
// #include <stdlib.h>

const int g_backlogSize = 50;

#define PORT 8080


void RunServer(std::string ip_addr) {
    ChatServiceImpl service;
    ServerBuilder builder;
    std::string server_addr = ip_addr+":"+std::to_string(PORT);
    builder.AddListeningPort(server_addr, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_addr << std::endl;
    server->Wait();
    std::cout << "Wait finished?" << std::endl;
}

int main (int argc, char const* argv[]) {
 
    // For getting host IP address we followed tutorial found here: 
    //      https://www.tutorialspoint.com/how-to-get-the-ip-address-of-local-computer-using-c-cplusplus
    char host[256];
    char *IP;
    hostent *host_entry;
    int hostname;
    hostname = gethostname(host, sizeof(host)); //find the host name
    host_entry = gethostbyname(host); //find host information
    IP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])); //Convert into IP string
    RunServer(IP);

    // std::cout << "Terminating" << std::endl;
    // std::vector<std::thread> threads;
    
    // while (1) {

    //     // Pass new socket to a thread to handle commands
    //     std::thread clientThread(handleClient, new_socket);

    //     threadDictionary[clientThread.get_id()] = clientThread.native_handle();

    //     clientThread.detach();

    // };

    return 0;
}