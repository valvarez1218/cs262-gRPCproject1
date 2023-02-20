// #include "../messageTypes.h"
#include "establishConnection.h"

// Boolean determining whether the user has logged in
bool USER_LOGGED_IN = false;
// Boolean determining whether program is still running
bool g_ProgramRunning = true;

std::string loggedInErrorMsg(std::string operationAttempted) {
    return "User must be logged in to perform " + operationAttempted;
}

void createAccount(int socket_fd, CreateAccountMessage &create_account_message) {
    if (USER_LOGGED_IN) {
        throw std::runtime_error("Cannot create account if already logged in.");
    }

    int valsent = send(socket_fd, &create_account_message, sizeof(CreateAccountMessage), 0);

    CreateAccountReply serverReply;
    int valread = read(socket_fd, &serverReply, sizeof(CreateAccountReply));

    if (valread == -1) {
        throw std::runtime_error("Error reading server response from socket.");
    }

    if (serverReply.queryStatus == 1) {
        throw std::runtime_error("Username was taken, could not create account.");
    }

    std::cout << "Welcome " << create_account_message.userName << "!" << std::endl;

    USER_LOGGED_IN = true;
}


void login(int socket_fd, LoginMessage &login_message) {
    if (USER_LOGGED_IN) {
        throw std::runtime_error("Cannot log in if already logged in.");
    }

    send(socket_fd, &login_message, sizeof(LoginMessage), 0);
    LoginReply serverReply;
    int valread = read(socket_fd, &serverReply, sizeof(LoginReply));

    if (valread == -1) {
        throw std::runtime_error("Error reading server response from socket.");
    }

    if (serverReply.queryStatus == 1) {
        throw std::runtime_error("Incorrect username or password.");
    }
    std::cout << "Welcome " << login_message.userName << "!" << std::endl;
    USER_LOGGED_IN = true;
}


// ALL FUNCTIONS BELOW HERE REQUIRE USER TO BE LOGGED IN


void logout(int socket_fd, LogoutMessage &logout_message) {
    if (!USER_LOGGED_IN) {
        throw std::runtime_error(loggedInErrorMsg("logout"));
    }

    send(socket_fd, &logout_message, sizeof(LogoutMessage), 0);
    USER_LOGGED_IN = false;
    g_ProgramRunning = false;
    std::cout << "Logged out, goodbye!" << std::endl;
}


void listUsers(int socket_fd, ListUsersMessage &list_users_message) {
    if (!USER_LOGGED_IN) {
        throw std::runtime_error(loggedInErrorMsg("list_users"));
    }

    send(socket_fd, &list_users_message, sizeof(ListUsersMessage), 0);

    ListUsersReply serverReply;
    
    try {
        // the parse function prints usernames to command line
        serverReply.readUsernames(socket_fd);
    } catch(std::runtime_error &e) {
        throw e;
    }
}


void sendMessage(int socket_fd, SendMessageMessage &send_message_message) {
    if (!USER_LOGGED_IN) {
        throw std::runtime_error(loggedInErrorMsg("send_message"));
    }

    send(socket_fd, &send_message_message, sizeof(SendMessageMessage), 0);

    SendMessageReply serverReply;
    int valread = read(socket_fd, &serverReply, sizeof(SendMessageReply));
    if (valread == -1) {
        throw std::runtime_error("Error reading SendMessageReply from server.");
    }
    if (serverReply.queryStatus == 1) {
        std::string errorMsg = "User " + std::string(send_message_message.recipientUsername) + " was not found.";
        throw std::runtime_error(errorMsg);
    } else {
        std::cout << "Message sent to " << std::string(send_message_message.recipientUsername) << std::endl;
    }

}


void queryNotifications(int socket_fd, QueryNotificationsMessage &query_notification_message) {
    if (!USER_LOGGED_IN) {
        throw std::runtime_error(loggedInErrorMsg("query_notifications"));
    }

    int valsent = send(socket_fd, &query_notification_message, sizeof(QueryNotificationsMessage), 0);

    QueryNotificationReply serverReply;
    serverReply.readNotifications(socket_fd);
}


void queryMessages(int socket_fd, QueryMessagesMessage &query_messages_message) {
    if (!USER_LOGGED_IN) {
        throw std::runtime_error(loggedInErrorMsg("query_messages"));
    }

    send(socket_fd, &query_messages_message, sizeof(QueryMessagesMessage), 0);

    QueryMessagesReply serverReply;
    try {
        int messagesRead = serverReply.readMessages(socket_fd);
        int firstIdx = serverReply.firstMessageIndex;
        MessagesSeenMessage msg(messagesRead, firstIdx);
        send(socket_fd, &msg, sizeof(MessagesSeenMessage), 0);
    } catch (std::runtime_error &e) {
        MessagesSeenMessage msg(0, -1);
        send(socket_fd, &msg, sizeof(MessagesSeenMessage), 0);
        throw e;
    }

}


void deleteAccount(int socket_fd, DeleteAccountMessage &delete_account_message) {
    if (!USER_LOGGED_IN) {
        throw std::runtime_error(loggedInErrorMsg("delete_account"));
    }

    send(socket_fd, &delete_account_message, sizeof(DeleteAccountMessage), 0);
    USER_LOGGED_IN = false;
    g_ProgramRunning = false;
    std::cout << "Account deleted, goodbye!" << std::endl;
}


// handle server messages
void readSocket() {
    opCode operation = REFRESH_REQUEST;
    send(server_socket, &operation, sizeof(opCode), 0);
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 500000;
    setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    while (true) {
        int valread = read(server_socket, &operation, sizeof(opCode));
        if (valread == -1) {
            std::cout << "No new updates." << std::endl;
            tv.tv_usec = 0;
            setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
            return;
        }
        // int valread = read(server_socket, &operation, sizeof(opCode));
        std::cout << "Read " << std::to_string(valread) << " bytes. Value is " << std::to_string(operation) << std::endl;

        switch (operation) {
            case NEW_MESSAGE:
                {
                    NewMessageMessage msg;
                    try {
                        msg.parse(server_socket);
                    } catch (std::runtime_error &e) {
                        throw e;
                    }
                }
                break;
            case FORCE_LOG_OUT:
                {
                    std::cout << "Log in on another device detected. Session ended." << std::endl;
                    USER_LOGGED_IN = false;
                    g_ProgramRunning = false;
                }
                break;
            default:
                std::cout << "Did not recognize operation from server" << std::endl;
        }
    }
}