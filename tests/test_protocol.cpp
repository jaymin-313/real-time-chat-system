#include <gtest/gtest.h>
#include "chat/CommonIncludes.h"

using namespace chat;

// Test fixture for Protocol tests
class ProtocolTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code for each test
    }
    
    void TearDown() override {
        // Cleanup code for each test
    }
};

// Test Message creation and basic properties
TEST_F(ProtocolTest, MessageCreation) {
    Message msg(MessageType::CHAT_MESSAGE, "testuser", "Hello World", "lobby");
    
    EXPECT_EQ(msg.type, MessageType::CHAT_MESSAGE);
    EXPECT_EQ(msg.sender, "testuser");
    EXPECT_EQ(msg.content, "Hello World");
    EXPECT_EQ(msg.room, "lobby");
    EXPECT_GT(msg.timestamp, 0);
}

// Test Message serialization and deserialization
TEST_F(ProtocolTest, MessageSerialization) {
    Message original(MessageType::PRIVATE_MESSAGE, "alice", "Secret message", "");
    original.recipient = "bob";
    
    // Serialize message
    std::string serialized = utils::serializeMessage(original);
    EXPECT_FALSE(serialized.empty());
    
    // Deserialize message
    Message deserialized = utils::deserializeMessage(serialized);
    
    // Verify all fields match
    EXPECT_EQ(deserialized.type, original.type);
    EXPECT_EQ(deserialized.sender, original.sender);
    EXPECT_EQ(deserialized.recipient, original.recipient);
    EXPECT_EQ(deserialized.room, original.room);
    EXPECT_EQ(deserialized.content, original.content);
    EXPECT_EQ(deserialized.timestamp, original.timestamp);
}

// Test UserInfo structure
TEST_F(ProtocolTest, UserInfoSerialization) {
    UserInfo user("testuser");
    user.current_room = "general";
    user.is_connected = true;
    
    // Test default values
    EXPECT_EQ(user.username, "testuser");
    EXPECT_EQ(user.current_room, "general");
    EXPECT_TRUE(user.is_connected);
    
    // TODO: Add serialization tests once implemented
}

// Test RoomInfo structure
TEST_F(ProtocolTest, RoomInfoCreation) {
    RoomInfo room("test-room", 50, false);
    
    EXPECT_EQ(room.room_name, "test-room");
    EXPECT_EQ(room.max_users, 50);
    EXPECT_FALSE(room.is_private);
    EXPECT_TRUE(room.users.empty());
}

// Test message formatting utilities
TEST_F(ProtocolTest, MessageFormatting) {
    Message chat_msg(MessageType::CHAT_MESSAGE, "alice", "Hello everyone!", "general");
    std::string formatted = utils::formatMessage(chat_msg);
    
    EXPECT_FALSE(formatted.empty());
    EXPECT_NE(formatted.find("alice"), std::string::npos);
    EXPECT_NE(formatted.find("Hello everyone!"), std::string::npos);
}

// Test different message types
TEST_F(ProtocolTest, MessageTypes) {
    // Test all message types can be created
    Message join_msg(MessageType::USER_JOIN, "user1", "joined", "lobby");
    EXPECT_EQ(join_msg.type, MessageType::USER_JOIN);
    
    Message leave_msg(MessageType::USER_LEAVE, "user1", "left", "lobby");
    EXPECT_EQ(leave_msg.type, MessageType::USER_LEAVE);
    
    Message server_msg(MessageType::SERVER_ANNOUNCEMENT, "server", "Welcome!", "");
    EXPECT_EQ(server_msg.type, MessageType::SERVER_ANNOUNCEMENT);
    
    Message heartbeat_msg(MessageType::HEARTBEAT, "client", "", "");
    EXPECT_EQ(heartbeat_msg.type, MessageType::HEARTBEAT);
}
