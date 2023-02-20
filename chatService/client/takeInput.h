// #include "../messageTypes.h"
#include "userOperations.h"

#include <vector>
#include <unordered_map>

// DECLARING GLOBALS
// a map from user commands to operation codes
const std::unordered_map<std::string, opCode> operationMap {
    {"create_account",              CREATE_ACCOUNT},
    {"login",                       LOGIN},
    {"logout",                      LOGOUT},
    {"list_users",                  LIST_USERS},
    {"send_message",                SEND_MESSAGE},
    {"query_notifications",         QUERY_NOTIFICATIONS},
    {"query_messages",              QUERY_MESSAGES},
    {"delete_account",              DELETE_ACCOUNT},
};


// Limit to the number of characters the command line will read
const size_t g_InputLimit = 1303;

// This function simply takes input from the user and checks that it does not go
//     over the character limit
bool takeInput (char (&inputBuffer)[g_InputLimit]) {
    std::cout << "chat262$ ";
    std::cin.getline(inputBuffer, g_InputLimit);

    if (std::cin.gcount() > g_InputLimit - 3) {
        std::cout << "The character limit is 1300 characters!" << std::endl;
        return false;
    }

    return true;
}

// This is a helper function for vectorizing the user input
std::vector<std::string> extractWords (std::string inputString) {
    // Convert remainder of string into vector of strings
    std::string delimiters = " \t";
    std::vector<std::string> remainingInputVector;
    int start = inputString.find_first_not_of(delimiters);
    int end = inputString.find_first_of(delimiters, start);
    while (end != std::string::npos && start != std::string::npos) {
        std::string substr = inputString.substr(start, end - start);
        remainingInputVector.push_back(substr);
        start = inputString.find_first_not_of(delimiters, end);
        end = inputString.find_first_of(delimiters, start);
    }

    if (start != std::string::npos) {
        std::string substr = inputString.substr(start, std::string::npos);
        if (substr.size() != 0) {
            remainingInputVector.push_back(substr);
        }
    }

    return remainingInputVector;
}

// This function takes a string and returns input as vector of strings as the message classes would expect it
std::vector<std::string> makeStringVector (std::string inputString) {
    int firstQuoteIdx = inputString.find_first_of("'\"");
    if (firstQuoteIdx == std::string::npos) {
        std::vector<std::string> words = extractWords(inputString);
        if (words[0] == "send_message") {
            throw std::invalid_argument("Format Error: send_message command expects message wrapped in quotations.");
        }
        return words;
    }

    std::string substr1 = inputString.substr(0, firstQuoteIdx);

    int lastQuoteIdx = inputString.find_last_of("'\"");
    if (lastQuoteIdx == firstQuoteIdx) {
        throw std::invalid_argument("Format Error: Closing quotation mark expected.");
    }
    std::string substr2 = inputString.substr(firstQuoteIdx+1, lastQuoteIdx-firstQuoteIdx-1);

    int start_last = inputString.find_first_not_of(" ", lastQuoteIdx+1);
    if (start_last != std::string::npos) {
        throw std::invalid_argument("Format Error: Should not have characters after the final quotation mark.");
    }

    // Convert remainder of string into vector of strings
    std::vector<std::string> remainingInputVector = extractWords(substr1);
    remainingInputVector.push_back(substr2);

    return remainingInputVector;
}


void printUsage();

struct ParsedInput {
    opCode operation;
    std::vector<std::string> arguments;

    // Default constructor initialized nothing
    ParsedInput(){}

    ParsedInput(opCode op, std::vector<std::string> args) {
        operation = op;
        arguments = args;
    }
};

// This function will parse the user input and return the corresponding operation code
ParsedInput parseInput (std::string userInput) {
    if (userInput.size() == 0) {
        return ParsedInput(-1, {});
    }

    // Convert input string into vector of strings
    std::vector<std::string> inputVector;
    try {
        inputVector = makeStringVector(userInput);
    } catch(std::invalid_argument &e) {
        throw e;
    }

    // check that first token is operation recognized by program
    std::string firstToken = inputVector[0];
    if (operationMap.find(firstToken) == operationMap.end()) {
        std::string errorMessage = "'";
        errorMessage += firstToken;
        errorMessage += "' is not a defined operation.";
        throw std::invalid_argument(errorMessage);
    }
    opCode operation = operationMap.at(firstToken);
    // remove operation from beginning of input vector
    std::vector<std::string> remainingInputVector(inputVector.begin()+1, inputVector.end());

    return ParsedInput(operation, remainingInputVector);
}


void printUsage() {
    std::cout << "Chat Usage:" << std::endl;
    std::cout << "  If not logged in:" << std::endl;
    std::cout << "\n      - create_account username password:                 creates account and logs you in." << std::endl;
    std::cout << "\n      - login username password:                          logs you into existing account" << std::endl;
    std::cout << "\n  If logged in:" << std::endl;
    std::cout << "\n      - logout:                                           logs you out and closes application." << std::endl;
    std::cout << "\n      - list_users prefix:                                lists all users with prefix to command line, if no prefix given lists all users" << std::endl;
    std::cout << "\n      - send_message username \"message_content\":        sends message content to specified user" << std::endl;
    std::cout << "\n      - query_notifications:                              prints notifications as 'user: # message(s)'" << std::endl;
    std::cout << "\n      - query_messages username:                          prints out messages with specified user, up to 20 at at time" << std::endl;
    std::cout << "\n      - delete_account username password:                 delets account associated with username and exits application." << std::endl;
}