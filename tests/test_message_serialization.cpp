#include <gtest/gtest.h>
#include "chat/CommonIncludes.h"
#include <sstream>

using namespace chat;

class SerializationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup for serialization tests
    }
    
    void TearDown() override {
        // Cleanup
    }
    
    // Helper function to test round-trip serialization
    template<typename T>
    void testRoundTrip(const T& original) {
        std::ostringstream oss;
        boost::archive::text_oarchive oa(oss);
        oa << original;
        
        std::string serialized_data = oss.str();
        EXPECT_FALSE(serialized_data.empty());
        
        std::istringstream iss(serialized_data);
        boost::archive::text_iarchive ia(iss);
        T deserialized;
        ia >> deserialized;
        
        // Note: Actual comparison implementation depends on operator== for each type
        // This is a template for the test structure
    }
};

TEST_F(SerializationTest, MessageSerializationRoundTrip) {
    Message msg(MessageType::CHAT_MESSAGE, "testuser", "Test message", "testroom");
    msg.recipient = "targetuser";
    
    // Test the round-trip using utility functions
    std::string serialized = utils::serializeMessage(msg);
    Message deserialized = utils::deserializeMessage(serialized);
    
    EXPECT_EQ(msg.type, deserialized.type);
    EXPECT_EQ(msg.sender, deserialized.sender);
    EXPECT_EQ(msg.recipient, deserialized.recipient);
    EXPECT_EQ(msg.room, deserialized.room);
    EXPECT_EQ(msg.content, deserialized.content);
    EXPECT_EQ(msg.timestamp, deserialized.timestamp);
}

TEST_F(SerializationTest, EmptyMessageFields) {
    Message msg(MessageType::HEARTBEAT, "", "", "");
    
    std::string serialized = utils::serializeMessage(msg);
    Message deserialized = utils::deserializeMessage(serialized);
    
    EXPECT_EQ(deserialized.type, MessageType::HEARTBEAT);
    EXPECT_TRUE(deserialized.sender.empty());
    EXPECT_TRUE(deserialized.recipient.empty());
    EXPECT_TRUE(deserialized.room.empty());
    EXPECT_TRUE(deserialized.content.empty());
}

TEST_F(SerializationTest, LargeMessageContent) {
    std::string large_content(1000, 'A'); // 1000 'A' characters
    Message msg(MessageType::CHAT_MESSAGE, "user", large_content, "room");
    
    std::string serialized = utils::serializeMessage(msg);
    Message deserialized = utils::deserializeMessage(serialized);
    
    EXPECT_EQ(deserialized.content, large_content);
    EXPECT_EQ(deserialized.content.size(), 1000);
}

TEST_F(SerializationTest, SpecialCharacters) {
    std::string special_content = "Hello 你好 🌍 \n\t\r Special chars!";
    Message msg(MessageType::CHAT_MESSAGE, "user", special_content, "room");
    
    std::string serialized = utils::serializeMessage(msg);
    Message deserialized = utils::deserializeMessage(serialized);
    
    EXPECT_EQ(deserialized.content, special_content);
}

// TODO: Add UserInfo and RoomInfo serialization tests
TEST_F(SerializationTest, DISABLED_UserInfoSerialization) {
    // Placeholder for UserInfo serialization tests
    // Will be implemented once UserInfo operator== is available
    FAIL() << "UserInfo serialization tests not yet implemented";
}

TEST_F(SerializationTest, DISABLED_RoomInfoSerialization) {
    // Placeholder for RoomInfo serialization tests  
    // Will be implemented once RoomInfo operator== is available
    FAIL() << "RoomInfo serialization tests not yet implemented";
}
