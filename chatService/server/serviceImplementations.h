#include "../chatService.grpc.pb.h"

using chatservice::ChatService;
using grpc::Status;

class ChatServiceImpl final : public chatservice::ChatService::Service {
    explicit ChatServiceImpl() {}

    // TODO
    Status CreateAccount() {

    }

    // TODO
    Status Login() {

    }

    // TODO
    Status Logout() {

    }

    // TODO
    Status ListUsers() {

    }

    // TODO
    Status SendMessage() {

    }

    // TODO
    Status QueryNotifications() {

    }

    // TODO
    Status QueryMessages() {

    }

    // TODO
    Status DeleteAccount() {

    }

    // TODO: might not need this one either?
    Status MessagesSeen() {

    }

    // TODO: might not need this one, it's from server to client
    Status NewMessage() {

    }
};