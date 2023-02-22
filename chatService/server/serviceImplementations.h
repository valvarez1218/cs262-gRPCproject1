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
using chatservice::MessagesSeenReply;

class ChatServiceImpl final : public chatservice::ChatService::Service {
    private:
        std::mutex mu_;
        // This might be where we store the conversations open per user or something

    public:
        explicit ChatServiceImpl() {}

        Status CreateAccount(ServerContext* context, const CreateAccountMessage* create_account_message, 
                            CreateAccountReply* server_reply) {
            // Mutex lock, check for existing users, add user, etc.
            if (userTrie.userExists(create_account_message->username())) {
                std::string errorMsg = "Username '" + create_account_message->username() + "' already exists.";
                server_reply->set_errormsg(errorMsg);
                server_reply->set_createaccountsuccess(false);
            } else {
                // Update storage with new user
                std::string username = create_account_message->username();
                std::string password = create_account_message->password();
                userTrie_mutex.lock();
                userTrie.addUsername(username, password);
                userTrie_mutex.unlock();

                activeUser_mutex.lock();
                activeUsers.insert(username);
                activeUser_mutex.unlock();

                std::cout << "Added user " << username << std::endl;
                server_reply->set_createaccountsuccess(true);
            }
            return Status::OK;
        }


        Status Login(ServerContext* context, const LoginMessage* login_message, LoginReply* server_reply) {
            // Check for existing user and verify password
            std::string username = login_message->username();
            std::string password = login_message->password();

            userTrie_mutex.lock();
            bool verified = userTrie.verifyUser(username, password);
            userTrie_mutex.unlock();
            
            if (verified) {
                server_reply->set_loginsuccess(true);
                activeUser_mutex.lock();
                activeUsers.insert(username);
                activeUser_mutex.unlock();
            } else {
                server_reply->set_loginsuccess(false);
                server_reply->set_errormsg("Incorrect username or password.");
            }

            return Status::OK;
        }


        Status Logout(ServerContext* context, const LogoutMessage* logout_message, LogoutReply* server_reply) {
            activeUser_mutex.lock();
            activeUsers.erase(logout_message->username());
            activeUser_mutex.unlock();
            return Status::OK;
        }


        Status ListUsers(ServerContext* context, const QueryUsersMessage* query, ServerWriter<User>* writer) {
            std::string prefix = query->username();
            std::vector<std::string> usernames;
            userTrie_mutex.lock();
            try {
                usernames = userTrie.returnUsersWithPrefix(prefix);
            } catch (std::runtime_error &e) {
                std::cout << e.what() << std::endl;
                usernames = {};
            }
            userTrie_mutex.unlock();

            for (std::string username : usernames) {
                User user;
                user.set_username(username);
                writer->Write(user);
            }
            return Status::OK;
        }


        Status SendMessage(ServerContext* context, const ChatMessage* msg, SendMessageReply* server_reply) {
            bool userExists = userTrie.userExists(msg->recipientusername());

            if (userExists) {
                std::cout << "Adding message to storage from '" << msg->senderusername() << "' to '" << msg->recipientusername() << "'" << std::endl;
                // Add message to messages dictionary
                std::string sender = msg->senderusername();
                std::string recipient = msg->recipientusername();
                std::string content = msg->msgcontent();
                UserPair userPair(sender, recipient);
                messagesDictionary[userPair].addMessage(sender, recipient, content);

                activeUser_mutex.lock();
                if (activeUsers.find(recipient) != activeUsers.end()) {
                    queuedOperations_mutex.lock();
                    Notification note;
                    note.set_user(sender);
                    queuedOperationsDictionary[recipient].push_back(note);
                    queuedOperations_mutex.unlock();
                }
                activeUser_mutex.unlock();

                server_reply->set_messagesent(true);
            } else {
                std::string errormsg = "Tried to send a message to a user that doesn't exist '" + msg->recipientusername() + "'";
                server_reply->set_errormsg(errormsg);
            }
            return Status::OK;
        }


        Status QueryNotifications(ServerContext* context, const QueryNotificationsMessage* query, 
                                ServerWriter<Notification>* writer) {

            std::string clientUsername = query->user();
            std::vector<std::pair<char [g_UsernameLimit], char> > notifications = conversationsDictionary.getNotifications(clientUsername);
            
            for (auto notification : notifications) {
                std::cout << "Username: " << notification.first << ", " << std::to_string(notification.second) << " notifications" << std::endl;
                Notification note;
                note.set_numberofnotifications(notification.second);
                note.set_user(notification.first);
                writer->Write(note);
            }
            return Status::OK;
        }


        Status QueryMessages(ServerContext* context, const QueryMessagesMessage* query, 
                            ServerWriter<ChatMessage>* writer) {
            std::cout << "Getting messages between '" << query->clientusername() << "' and '"<< query->otherusername() << "'" << std::endl;

            // Get stored messages depending on if the client has the conversation open
            UserPair userPair(query->clientusername(), query->otherusername());
            int lastMessageDeliveredIndex = -1;
            CurrentConversation currentConversation = currentConversationsDict[query->clientusername()];
            if (currentConversation.username == query->otherusername()) {
                lastMessageDeliveredIndex = currentConversation.messagesSentStartIndex;
            } else {
                currentConversation.username = query->otherusername();
            }

            GetStoredMessagesReturnValue returnVal = messagesDictionary[userPair].getStoredMessages(query->clientusername(), lastMessageDeliveredIndex);

            // Update current conversation information
            currentConversation.messagesSentStartIndex = returnVal.firstMessageIndex;
            currentConversation.messagesSentEndIndex = returnVal.lastMessageIndex;
            currentConversationsDict[query->clientusername()] = currentConversation;

            for (auto message : returnVal.messageList) {
                writer->Write(message);
            }

            return Status::OK;
        }

        Status DeleteAccount(ServerContext* context, const DeleteAccountMessage* delete_acount_message,
                            DeleteAccountReply* server_reply) {
            std::cout << "Deleting account of '" << delete_acount_message->username() << "'" << std::endl;
            // Flag user account as deleted in trie
            userTrie_mutex.lock();
            try {
                userTrie.deleteUser(delete_acount_message->username());
            } catch (std::runtime_error &e) {
                std::cout << e.what() << std::endl;
                server_reply->set_errormsg(e.what());
                server_reply->set_deletedaccount(false);
                return Status::OK;
            }
            userTrie_mutex.unlock();

            currentConversationsDict.erase(delete_acount_message->username());
            server_reply->set_deletedaccount(true);
            return Status::OK;
        }


        Status MessagesSeen(ServerContext* context, const MessagesSeenMessage* msg, MessagesSeenReply* reply) {
            UserPair userPair(msg->clientusername(), msg->otherusername());
            int startIdx = currentConversationsDict[msg->clientusername()].messagesSentStartIndex;
            // if (messagesSeenMessage.startingIndex == -1)
            messagesDictionary[userPair].setRead(startIdx,
                                                    startIdx+msg->messagesseen() - 1, msg->clientusername());

            return Status::OK;
        }

        Status RefreshClient(ServerContext* context, const RefreshRequest* request, RefreshResponse* reply) {
            std::cout << "Refreshing for " << request->clientusername() << std::endl;
            if (queuedOperationsDictionary.find(request->clientusername()) != queuedOperationsDictionary.end()) {
                std::cout << "Running queued operations for '" << request->clientusername() << "'" << std::endl;
                for (Notification note : queuedOperationsDictionary[request->clientusername()]) {
                    Notification* n = reply->add_notifications();
                    n->set_user(note.user());
                }

                queuedOperationsDictionary.erase(request->clientusername());
            }
            return Status::OK;
        }
};