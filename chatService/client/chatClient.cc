#include "../../build/chatService.pb.h"
#include "../../build/chatService.grpc.pb.h"

int main() {
    chatService::chatMessage msg;
    msg.set_msgcontent("Hello there");
    msg.set_senderusername("Vic");
    std::cout << msg.senderusername() << " : " << msg.msgcontent() << std::endl;

    return 0;
}