#include <gtest/gtest.h>
#include "../chatService/client/takeInput.h"

std::vector<std::string> makeVectorHelper(std::string str) {
    try {
        return makeStringVector(str);
    } catch (std::invalid_argument &e) {
        return std::vector<std::string> {e.what()};
    }
}

TEST(TakeInput, ParsingUserInput) {
    std::string test1 = "send_message username \"Hello there\"";
    std::vector<std::string> expected1 {"send_message", "username", "Hello there"};
    EXPECT_EQ(makeStringVector(test1), expected1);

    std::string test2 = "one two three";
    std::vector<std::string> expected2 {"one", "two", "three"};
    EXPECT_EQ(makeStringVector(test2), expected2);

    std::string test3 = "one two \"three";
    std::vector<std::string> expected3 {"Format Error: Closing quotation mark expected."};
    EXPECT_EQ(makeVectorHelper(test3), expected3);

    std::string test4 = "one two \"three\" four";
    std::vector<std::string> expected4 {"Format Error: Should not have characters after the final quotation mark."};
    EXPECT_EQ(makeVectorHelper(test4), expected4);

    std::string test5 = "";
    std::vector<std::string> expected5 {};
    EXPECT_EQ(makeVectorHelper(test5), expected5);

    std::string test6 = "send_message blank what up dude";
    std::vector<std::string> expected6 {"Format Error: send_message command expects message wrapped in quotations."};
    EXPECT_EQ(makeVectorHelper(test6), expected6);
}