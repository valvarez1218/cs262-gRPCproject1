#include <stdio.h>
#include <string>
#include <stdexcept>

// Client->Server Messages 
#define CREATE_ACCOUNT              1
#define LOGIN                       2
#define LOGOUT                      3
#define LIST_USERS                  4
#define SEND_MESSAGE                5
#define QUERY_NOTIFICATIONS         6
#define QUERY_MESSAGES              7
#define DELETE_ACCOUNT              8
#define MESSAGES_SEEN               9

// Server->Client Messages
#define NEW_MESSAGE                 10
#define FORCE_LOG_OUT               20

// Server->Client Replies 
#define CREATE_ACCOUNT_REPLY        11
#define LOGIN_REPLY                 12
#define LIST_USERS_REPLY            13
#define SEND_MESSAGE_REPLY          14
#define QUERY_NOTIFICATIONS_REPLY   15
#define QUERY_MESSAGES_REPLY        16
#define MESSAGES_SEEN_REPLY         18

#define REFRESH_REQUEST             21
// #define NEW_MESSAGE_REPLY           19
#define HELP                        22

// This is a value corresponding to the supported operations
typedef char opCode;

// global variables
const size_t g_UsernameLimit = 32;
const size_t g_PasswordLimit = 31;
const size_t g_MessageLimit = 1001;
const size_t g_MessageQueryLimit = 20;

const size_t g_ClientUsernameLimit = g_UsernameLimit - 1;
const size_t g_ClientPasswordLimit = g_PasswordLimit - 1;
const size_t g_ClientMessageLimit = g_MessageLimit - 1;

const std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";


// check that character follows allowed alphabet
bool validString(std::string inputString) {
    // Check that all characters are from alphabet
    int found = inputString.find_first_not_of(alphabet);
    if (found != std::string::npos) {
        return false;
    }
    return true;
}

void validateField(std::string fieldName, std::string fieldValue, const size_t fieldLimit) {
    if (fieldValue.size() < 1) {
        std::string errorMsg = fieldName + " cannot be empty string.";
        throw std::invalid_argument(errorMsg);
    }
    if (!validString(fieldValue)) {
        std::string errorMsg = fieldName + " must be alphanumeric.";
        throw std::invalid_argument(errorMsg);
    }
    if (fieldValue.size() > fieldLimit) {
        std::string errorMsg = "The " + fieldName + " field takes input of at most " + std::to_string(fieldLimit) +
            " characters.\n '" + fieldValue + "' is too long.";
        throw std::invalid_argument(errorMsg);
    }
}