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
using chatservice::RefreshRequest;
using chatservice::RefreshResponse;

class ChatServiceImpl final : public chatservice::ChatService::Service {
    private:
        std::mutex mu_;
        // This might be where we store the conversations open per user or something

    public:
        explicit ChatServiceImpl() {}

        // TODO
        Status CreateAccount(ServerContext* context, const CreateAccountMessage* create_account_message, 
                            CreateAccountReply* server_reply) {
            // Mutex lock, check for existing users, add user, etc.
            if (userTrie.userExists(create_account_message->username())) {
                std::string errorMsg = "Username '" + create_account_message->username() + "' already exists.";
                server_reply->set_errormsg(errorMsg);
            } else {
                // Update storage with new user
                userTrie_mutex.lock();
                std::string username = create_account_message->username();
                std::string password = create_account_message->password();
                userTrie.addUsername(username, password);
                userTrie_mutex.unlock();


                // TODO: Do we need the handlerDescriptor anymore?
                std::pair<std::thread::id, int> handlerDescriptor(thread_id, client_fd);
                socketDictionary_mutex.lock();
                socketDictionary[username] = handlerDescriptor;
                socketDictionary_mutex.unlock();

                std::cout << "Username '" << username << "' added with client_fd: " << std::to_string(client_fd) << ", and thread id: "<< thread_id << std::endl;
                return Status::OK;
            }           
        }

        // TODO
        Status Login(ServerContext* context, const LoginMessage* login_message, LoginReply* server_reply) {
            // Check for existing user and verify password
            std::string username = login_message->username();
            std::string password = login_message->password();

            userTrie_mutex.lock();
            bool verified = userTrie.verifyUser(username, password);
            userTrie_mutex.unlock();
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

        // TODO: send client updates
        Status RefreshClient(ServerContext* context, const RefreshRequest* request, RefreshResponse* reply) {

        }
};