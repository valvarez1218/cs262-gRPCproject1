#include <gtest/gtest.h>
#include <thread>
#include "../chatService/client/takeInput.h"

TEST(MessageSizeTest, CheckSizeOfMessage) {

	// CreateAccountMessage create_account_message;
	EXPECT_EQ(sizeof(CreateAccountMessage), sizeof(opCode)+g_UsernameLimit+g_PasswordLimit);
	EXPECT_EQ(sizeof(LoginMessage), sizeof(opCode)+g_UsernameLimit+g_PasswordLimit);
	EXPECT_EQ(sizeof(LogoutMessage), sizeof(opCode));
	EXPECT_EQ(sizeof(ListUsersMessage), sizeof(opCode)+g_UsernameLimit);
	EXPECT_EQ(sizeof(SendMessageMessage), sizeof(opCode)+g_UsernameLimit+g_MessageLimit);
	EXPECT_EQ(sizeof(QueryNotificationsMessage), sizeof(opCode));
	EXPECT_EQ(sizeof(QueryMessagesMessage), sizeof(opCode)+g_UsernameLimit);
	EXPECT_EQ(sizeof(DeleteAccountMessage), sizeof(opCode));
	EXPECT_EQ(sizeof(MessagesSeenMessage), 2*sizeof(char)+sizeof(opCode));
	EXPECT_EQ(sizeof(NewMessageMessage), sizeof(opCode)+g_UsernameLimit);
	EXPECT_EQ(sizeof(CreateAccountReply), sizeof(char)+sizeof(opCode));
	EXPECT_EQ(sizeof(LoginReply), sizeof(char)+sizeof(opCode));
	EXPECT_EQ(sizeof(Username), g_UsernameLimit);

	ListUsersReply list_users_reply;
	list_users_reply.numberOfUsers = 4;
	EXPECT_EQ(sizeof(list_users_reply), sizeof(char)+sizeof(opCode));

	EXPECT_EQ(sizeof(SendMessageReply), sizeof(opCode)+sizeof(char));
	
	int numberUsers = 4;
	std::pair<char [g_UsernameLimit], char> notification1;
	std::pair<char [g_UsernameLimit], char> notification2;
	std::pair<char [g_UsernameLimit], char> notification3;
	std::pair<char [g_UsernameLimit], char> notification4;
	std::vector<std::pair<char [g_UsernameLimit], char>> notificationsList;
	notificationsList.push_back(notification1);
	notificationsList.push_back(notification2);
	notificationsList.push_back(notification3);
	notificationsList.push_back(notification4);
	QueryNotificationReply query_notifications_reply(numberUsers);
	EXPECT_EQ(sizeof(query_notifications_reply), sizeof(opCode)+sizeof(char));

	EXPECT_EQ(sizeof(ReturnMessage), g_UsernameLimit+g_MessageLimit);

	ReturnMessage msg1;
	ReturnMessage msg2;
	ReturnMessage msg3;
	ReturnMessage msg4;
	std::vector<ReturnMessage> messageList{msg1, msg2, msg3, msg4};
	QueryMessagesReply query_messages_reply(4, -1);
	EXPECT_EQ(sizeof(query_messages_reply), 2*sizeof(char)+sizeof(opCode));

	EXPECT_EQ(sizeof(ForceLogOutReply), sizeof(opCode));
}