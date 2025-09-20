#pragma once

#include "chat/CommonIncludes.h"
#include <memory>
#include <atomic>

namespace chat {
    namespace server {
        
        // Forward declaration
        class ChatServer;
        class ChatRoom;
        
        class ChatSession : public std::enable_shared_from_this<ChatSession> {
        public:
            explicit ChatSession(tcp::socket socket, ChatServer& server);
            ~ChatSession();
            
            // Session lifecycle
            void start();
            void stop();
            bool isActive() const { return active_; }
            
            // User information
            const std::string& getUsername() const { return username_; }
            void setUsername(const std::string& username) { username_ = username; }
            
            const std::string& getCurrentRoom() const { return current_room_; }
            void setCurrentRoom(const std::string& room) { current_room_ = room; }
            
            // Message handling
            void sendMessage(const Message& message);
            void deliverMessage(const Message& message);
            
            // Connection information
            std::string getRemoteAddress() const;
            std::chrono::steady_clock::time_point getConnectTime() const { return connect_time_; }
            
            // Statistics
            std::size_t getMessagesSent() const { return messages_sent_; }
            std::size_t getMessagesReceived() const { return messages_received_; }
            
        private:
            // Core components
            tcp::socket socket_;
            ChatServer& server_;
            std::atomic<bool> active_;
            
            // User state
            std::string username_;
            std::string current_room_;
            std::chrono::steady_clock::time_point connect_time_;
            
            // Message queuing for async sending
            std::queue<Message> outgoing_messages_;
            std::mutex outgoing_mutex_;
            std::atomic<bool> write_in_progress_;
            
            // Statistics
            std::atomic<std::size_t> messages_sent_;
            std::atomic<std::size_t> messages_received_;
            
            // Receive buffer management
            enum { HEADER_LENGTH = 4 };
            std::array<char, HEADER_LENGTH> receive_header_buffer_;
            std::vector<char> receive_body_buffer_;
            
            // Private methods
            void startReceiveHeader();
            void handleReceiveHeader(const error_code& ec, std::size_t bytes_transferred);
            void startReceiveBody(std::uint32_t body_length);
            void handleReceiveBody(const error_code& ec, std::size_t bytes_transferred);
            
            void startSendMessage();
            void handleSendMessage(const error_code& ec, std::size_t bytes_transferred);
            
            void processReceivedMessage(const std::string& message_data);
            void handleConnectionError(const error_code& ec);
            void cleanup();
            
            // Message processing
            void processMessage(const Message& message);
            void sendWelcomeMessage();
            void sendGoodbyeMessage();
        };
        
    } // namespace server
} // namespace chat
