#include "serviceImplementations.h"

// Reads from socket and perform operations based on what it read
void handleClient(int client_fd) {
    int valread;
    CurrentConversation currentConversation;
    char clientUsername[g_UsernameLimit];
    std::thread::id thread_id = std::this_thread::get_id();

    std::cout << "thread id: " << thread_id << std::endl;

    std::cout << "New thread up!" << std::endl;

    while (true) {
        // Run queued operations if they exist

        std::cout << "beginning of loop" << std::endl;
        
        for (auto const& entry : forceLogoutDictionary) {
            std::cout << "Entry for logoutdict: " << std::to_string(entry.first) << std::endl;
        }

        // Checks for forced exit TODO: MAKE WORK
        if (forceLogoutDictionary.find(client_fd) != forceLogoutDictionary.end()) {
            std::cout << clientUsername << " forced to exit" << std::endl;
            forceLogoutDictionary.erase(client_fd);
            strcpy(threadExitReturnVal, "Thread exited");
            pthread_exit(threadExitReturnVal);
        }

        // Checks for queued operations
        // if (queuedOperationsDictionary.find(client_fd) != queuedOperationsDictionary.end()) {
        //     std::cout << "Running queued operations for '" << clientUsername << "'" << std::endl;
        //     for (int i = 0; i < queuedOperationsDictionary[client_fd].size(); i++) {
        //         NewMessageMessage newMessageMessage = queuedOperationsDictionary[client_fd][i];

        //         send(client_fd, &newMessageMessage, sizeof(newMessageMessage), 0);
        //         std::cout << "Sent message to '" << clientUsername << "'" << std::endl;

        //     }

        //     queuedOperationsDictionary.erase(client_fd);
        // }
        
        opCode operation;
        valread = read(client_fd, &operation, sizeof(opCode));

        // Cleans up if the client is disconnected
        if (valread == 0) {
            std::cout << "Client disconnected" << std::endl;
            cleanup(clientUsername, thread_id, client_fd);
            strcpy(threadExitReturnVal, "Thread exited");
            pthread_exit(threadExitReturnVal);
        }

        std::cout << "We've read an operation code!" << std::to_string(operation) << std::endl;

        switch (operation) {
            case CREATE_ACCOUNT:
            {
                // // Parse Message
                // CreateAccountMessage createAccountMessage;
                // createAccountMessage.parse(client_fd);
                // int queryResult = 0; // No error

                // std::cout << "We've parse the createAccountMessage" << std::endl;
                // // Verify username
                // std::string username = createAccountMessage.userName;
                // std::string password = createAccountMessage.password;



                if (userTrie.userExists(username)) {
                    std::cout << "Username '" << clientUsername << "' already existed." << std::endl;
                    queryResult = 1; // Username exists already, 
                } else {
                    // Update storage with new user
                    userTrie_mutex.lock();
                    userTrie.addUsername(username, password);
                    userTrie_mutex.unlock();

                    strcpy(clientUsername, createAccountMessage.userName);
                    std::pair<std::thread::id, int> handlerDescriptor(thread_id, client_fd);
                    socketDictionary_mutex.lock();
                    socketDictionary[std::string(clientUsername)] = handlerDescriptor;
                    socketDictionary_mutex.unlock();

                    std::cout << "Username '" << clientUsername << " added with client_fd: " << std::to_string(client_fd) << ", and thread id: "<< thread_id << std::endl;
                }


                // Create and send a response
                CreateAccountReply createAccountReply(queryResult);
                send(client_fd, &createAccountReply, sizeof(createAccountReply), 0);

            }
            break;
            case LOGIN:
            {
                // Parse message
                LoginMessage loginMessage;
                loginMessage.parse(client_fd);
                int queryResult = 0; // No error

                std::string username = loginMessage.userName;
                std::string password = loginMessage.password;
                std::cout << "We've parsed the loginMessage: " << username << std::endl;

                userTrie_mutex.lock();
                bool verified = userTrie.verifyUser(username, password);
                userTrie_mutex.unlock();

                std::cout << "We've verified the user" << std::endl;

                if (verified) {
                    // Check if person is already logged in
                    socketDictionary_mutex.lock();
                    threadDictionary_mutex.lock();

                    for (auto const& entry : socketDictionary) {
                        std::cout << "Entry for username: " << entry.first << ", with socket " << entry.second.second << std::endl;
                    }

                    bool foundEntry = socketDictionary.find(username) != socketDictionary.end();
                    std::cout << "Found entry? " << foundEntry << " username " << std::endl;

                    if (socketDictionary.find(username) != socketDictionary.end()) {
                        std::cout << "Person '" << username << " already logged in" << std::endl;
                        // Close socket
                        ForceLogOutReply forceLogOutReply;
                        send(socketDictionary[username].second, &forceLogOutReply, sizeof(forceLogOutReply), 0);
                        
                        // Kill thread handling original user session
                        // cleanup(username, socketDictionary[username].first, socketDictionary[username].second);
                        socketDictionary_mutex.unlock();
                        threadDictionary_mutex.unlock();

                        // TODO forceLogoutDictionary
                        forceLogoutDictionary[socketDictionary[username].second] = true;

                        // pthread_cancel(threadDictionary[socketDictionary[username].first]);
                         
                    }
                    // Logged in! Update storage accordingly
                    std::cout << "Logging in username '" << username << "'" << std::endl;
                    strcpy(clientUsername, username.c_str());
                    std::pair<std::thread::id, int> handlerDescriptor(thread_id, client_fd);
                    socketDictionary[std::string(clientUsername)] = handlerDescriptor;

                    threadDictionary_mutex.unlock();
                    socketDictionary_mutex.unlock();
                    std::cout << "We've logged in user " << clientUsername << std::endl;
                } else {
                    std::cout << "Username '" << clientUsername << "' not found." << std::endl;
                    queryResult = 1; // Username and password don't match, username doesn't exist, user deleted
                }

                // Create and send response
                LoginReply loginReply(queryResult);
                send(client_fd, &loginReply, sizeof(loginReply), 0);

            }
            break;
            case LOGOUT:
            {
                std::cout << "Logging out username '" << clientUsername << "'" << std::endl;

                // Closes thread
                // socketDictionary_mutex.lock();
                // threadDictionary_mutex.lock();
                // cleanup(std::string(clientUsername), thread_id, client_fd);
                // socketDictionary_mutex.unlock();
                // threadDictionary_mutex.unlock();

                // std::cout << "after cleanup" << std::endl;

                // strcpy(threadExitReturnVal, "Thread exited");
                // pthread_exit(threadExitReturnVal);

                // std::cout << "thread exited" << std::endl;
                
            } 
            break;
            case LIST_USERS:
            {
                // // Parse message
                // ListUsersMessage listUsersMessage;
                // listUsersMessage.parse(client_fd);

                // std::cout << "Getting users for '" << clientUsername << "'" << std::endl;
                // // Get users with prefix
                // userTrie_mutex.lock();
                // std::vector<std::string> usernames = userTrie.returnUsersWithPrefix(listUsersMessage.prefix);
                // userTrie_mutex.unlock();

                // std::cout << "got users from trie" << std::endl;

                // for (int i = 0; i < usernames.size(); i++) {
                //     std::cout << "User: " << usernames[i] << std::endl;
                // }

                // // Construct and send a reply
                // ListUsersReply listUsersReply(usernames.size());
                // send(client_fd, &listUsersReply, sizeof(listUsersReply), 0);

                // for (int i = 0; i < usernames.size(); i++) {
                //     Username user(usernames[i]);
                //     std::cout << user.username << std::endl;
                //     int valsent = send(client_fd, &user, sizeof(user), 0);
                //     std::cout << "Sent " << valsent << " bytes" << std::endl;
                // }

                // std::cout << "sent code" << std::endl;
               
                // std::cout << "sent data " <<  std::string(*usernames.data()) <<std::endl;

            }
            break;
            case SEND_MESSAGE:
            {
                // Read from socket into sendMessageMessage
                SendMessageMessage sendMessageMessage;
                sendMessageMessage.parse(client_fd);
                int queryResult = 0; // Message sent!

                bool userExists = userTrie.userExists(sendMessageMessage.recipientUsername);

                if (userExists) {
                    std::cout << "Adding message to storage from '" << clientUsername << "' to '" << sendMessageMessage.recipientUsername << "'" << std::endl;
                    // Add message to messages dictionary
                    UserPair userPair(clientUsername, sendMessageMessage.recipientUsername);
                    messagesDictionary[userPair].addMessage(clientUsername, sendMessageMessage.recipientUsername, sendMessageMessage.messageContent);

                    // Queue operation to send
                    // TOOD:   NewMessageMessage newMessageMessage(clientUsername);

                    NewMessageMessage newMessageMessage(clientUsername, sendMessageMessage.messageContent);
                    socketDictionary_mutex.lock();
                    if (socketDictionary.find(sendMessageMessage.recipientUsername) != socketDictionary.end()){
                        queuedOperations_mutex.lock();
                        std::cout << "Adding message for " << socketDictionary[sendMessageMessage.recipientUsername].second << " and not for " << clientUsername << std::endl;
                        queuedOperationsDictionary[socketDictionary[sendMessageMessage.recipientUsername].second].push_back(newMessageMessage);
                        queuedOperations_mutex.unlock();
                    }

                    socketDictionary_mutex.unlock();

                } else {
                    std::cout << "Tried to send a message to a user that doesn't exist '" << sendMessageMessage.recipientUsername << "'" << std::endl;
                     queryResult = 1; // User doesn't exist
                }

                SendMessageReply sendMessageReply(queryResult);
                send(client_fd, &sendMessageReply, sizeof(sendMessageReply), 0);

            }
            break;
            case QUERY_NOTIFICATIONS:
            {
                std::cout << "Getting notifications for '" << clientUsername << "'" << std::endl;
                // Retrieve notifications from the conversations dictionary
                std::vector<std::pair<char [g_UsernameLimit], char> > notifications = conversationsDictionary.getNotifications(clientUsername);

                // for (int i = 0; i < notifications.size(); i++) {
                //     std::cout << "Username: " << notifications[i].first << ", " << std::to_string(notifications[i].second) << " notifications" << std::endl;
                // }

                // Construct and send a reply
                QueryNotificationReply queryNotificationsReply(notifications.size());
                send(client_fd, &queryNotificationsReply, sizeof(queryNotificationsReply), 0);
                
                for (int i = 0; i < notifications.size(); i++) {
                    std::cout << "Username: " << notifications[i].first << ", " << std::to_string(notifications[i].second) << " notifications" << std::endl;
                    int valsent = send(client_fd, &notifications[i], sizeof(std::pair<char[g_UsernameLimit], char>),0);
                    std::cout << "Sent " << std::to_string(valsent) << std::endl;
                }
                // int valsent = send(client_fd, notifications.data(), notifications.size()*sizeof(std::pair<char [g_UsernameLimit], char>),0);

                // std::cout << valsent << " bytes sent" << std::endl;

            }
            break;
            case QUERY_MESSAGES:
            {
                // Parse message
                QueryMessagesMessage queryMessagesMessage;
                queryMessagesMessage.parse(client_fd);

                std::cout << "Getting messages between '" << clientUsername << "' and '"<< queryMessagesMessage.username << "'" << std::endl;

                // Get stored messages depending on if the client has the conversation open
                UserPair userPair(queryMessagesMessage.username, clientUsername);
                int lastMessageDeliveredIndex = -1;
                if (currentConversation.username == queryMessagesMessage.username) {
                    lastMessageDeliveredIndex = currentConversation.messagesSentStartIndex;
                } else {
                    strcpy(currentConversation.username, queryMessagesMessage.username);
                }

                GetStoredMessagesReturnValue returnVal = messagesDictionary[userPair].getStoredMessages(clientUsername,lastMessageDeliveredIndex);

                // Update current conversation information
                currentConversation.messagesSentStartIndex = returnVal.firstMessageIndex;
                currentConversation.messagesSentEndIndex = returnVal.lastMessageIndex;

                for (int i = 0; i < returnVal.messageList.size(); i++) {
                    std::cout << returnVal.messageList[i].senderUsername<< " : " << returnVal.messageList[i].messageContent << std::endl;
                }

                // Construct and send a reply
                QueryMessagesReply queryMessagesReply(returnVal.messageList.size(), returnVal.firstMessageIndex);
                send(client_fd, &queryMessagesReply, sizeof(queryMessagesReply), 0);

                

                for (int i = 0; i < returnVal.messageList.size(); i++) {
                    int valsent = send(client_fd, &returnVal.messageList[i], sizeof(ReturnMessage), 0);
                    std::cout << "QueryMessage sent " << std::to_string(valsent) << std::endl;
                }
                // send(client_fd, returnVal.messageList.data(), returnVal.messageList.size() * sizeof(ReturnMessage), 0);
                

            }
            break;
            case DELETE_ACCOUNT:
            {   
                std::cout << "Deleting account of '" << clientUsername << "'" << std::endl;
                // Flag user account as deleted in trie
                userTrie_mutex.lock();
                userTrie.deleteUser(std::string(clientUsername));
                userTrie_mutex.unlock();

                // Closes thread
                socketDictionary_mutex.lock();
                threadDictionary_mutex.lock();
                cleanup(clientUsername, thread_id, client_fd);
                socketDictionary_mutex.unlock();
                threadDictionary_mutex.unlock();

                strcpy(threadExitReturnVal, "Thread exited");
                pthread_exit(threadExitReturnVal);
 
            }
            break;
            case MESSAGES_SEEN:
            {
                // Parse message
                MessagesSeenMessage messagesSeenMessage;
                messagesSeenMessage.parse(client_fd);

                // Set messages as read
                UserPair userPair(currentConversation.username, clientUsername);
                // if (messagesSeenMessage.startingIndex == -1)
                messagesDictionary[userPair].setRead(messagesSeenMessage.startingIndex, messagesSeenMessage.startingIndex+messagesSeenMessage.messagesSeen - 1, clientUsername);

            }
            case REFRESH_REQUEST:
            {
                break;
            }
            break;
            default:
                std::cout << "Unrecognized operation\n";
                break;
        }
    }
}