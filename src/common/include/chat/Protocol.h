#pragma once

#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <string>
#include <vector>
#include <cstdint>
#include <chrono>

namespace chat {

    // Message types for our chat protocol
    enum class MessageType : uint8_t {
        USER_JOIN = 1,
        USER_LEAVE,
        CHAT_MESSAGE,
        PRIVATE_MESSAGE,
        ROOM_JOIN,
        ROOM_LEAVE,
        SERVER_ANNOUNCEMENT,
        HEARTBEAT
    };

    // Core message structure with Boost.Serialization support
    struct Message {
        MessageType type;
        std::string sender;
        std::string recipient;  // Empty for broadcast, specific for private
        std::string room;       // Room name, empty for global chat
        std::string content;
        uint64_t timestamp;     // Unix timestamp

        // Default constructor required for serialization
        Message() = default;

        Message(MessageType msg_type, const std::string& sender_name, 
                const std::string& message_content, const std::string& room_name = "")
            : type(msg_type), sender(sender_name), room(room_name), 
              content(message_content), timestamp(std::chrono::duration_cast<std::chrono::seconds>(
                  std::chrono::system_clock::now().time_since_epoch()).count()) {}

    private:
        friend class boost::serialization::access;
        
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version) {
            ar & type & sender & recipient & room & content & timestamp;
        }
    };

    // User information structure
    struct UserInfo {
        std::string username;
        std::string current_room;
        bool is_connected;

        UserInfo() = default;
        UserInfo(const std::string& name) : username(name), current_room("lobby"), is_connected(true) {}

    private:
        friend class boost::serialization::access;
        
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version) {
            ar & username & current_room & is_connected;
        }
    };

    // Room information structure
    struct RoomInfo {
        std::string room_name;
        std::vector<std::string> users;
        uint32_t max_users;
        bool is_private;

        RoomInfo() = default;
        RoomInfo(const std::string& name, uint32_t max = 100, bool private_room = false)
            : room_name(name), max_users(max), is_private(private_room) {}

    private:
        friend class boost::serialization::access;
        
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version) {
            ar & room_name & users & max_users & is_private;
        }
    };

} // namespace chat
