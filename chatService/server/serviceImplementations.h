#include "../chatService.grpc.pb.h"
#include "storage.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

using chatservice::ChatService;
// Messages
using chatservice::CreateAccountMessage;
using chatservice::LoginMessage;
using chatservice::LogoutMessage;
using chatservice::QueryUsersMessage;
using chatservice::ChatMessage;
using chatservice::QueryNotificationsMessage;
using chatservice::QueryMessagesMessage;
using chatservice::DeleteAccountMessage;
using chatservice::MessagesSeenMessage;
// Replies
using chatservice::CreateAccountReply;
using chatservice::LoginReply;
using chatservice::LogoutReply;
using chatservice::User;
using chatservice::SendMessageReply;
using chatservice::Notification;
using chatservice::DeleteAccountReply;
using chatservice::NewMessageReply;

class ChatServiceImpl final : public chatservice::ChatService::Service {
    explicit ChatServiceImpl() {}

    // TODO
    Status CreateAccount(ServerContext* context, const CreateAccountMessage* creat_account_message, 
                         CreateAccountReply* server_reply) {
        // Mutex lock, check for existing users, add user, etc.

    }

    // TODO
    Status Login(ServerContext* context, const LoginMessage* login_message, LoginReply* server_reply) {
        // Check for existing user and verify password

    }

    // TODO
    Status Logout(ServerContext* context, const LogoutMessage* logout_message, LogoutReply* server_reply) {
        // close file descriptor and thread

    }

    // TODO
    Status ListUsers(ServerContext* context, const QueryUsersMessage* query, ServerWriter<User>* writer) {
        // Mutex lock?
    }

    // TODO
    Status SendMessage(ServerContext* context, const ChatMessage* msg, SendMessageReply* server_reply) {

    }

    // TODO
    Status QueryNotifications(ServerContext* context, const QueryNotificationsMessage* query, 
                              ServerWriter<Notification>* writer) {

    }

    // TODO
    Status QueryMessages(ServerContext* context, const QueryMessagesMessage* query, 
                         ServerWriter<ChatMessage>* writer) {

    }

    // TODO
    Status DeleteAccount(ServerContext* context, const DeleteAccountMessage* delete_acount_message,
                         DeleteAccountReply* server_reply) {

    }

    // TODO: might not need this one either?
    Status MessagesSeen(ServerContext* context, const MessagesSeenMessage* msg, MessagesSeenMessage* reply) {

    }

    // TODO: might not need this one, it's from server to client
    Status NewMessage(ServerContext* context, const ChatMessage* msg, NewMessageReply* client_reply) {

    }
};