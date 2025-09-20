#pragma once

#include "chat/CommonIncludes.h"
#include "MessageHandler.h"
#include "UserInterface.h"
#include <memory>
#include <atomic>
#include <thread>
#include <queue>

namespace chat {
    namespace client {
        
        class ChatClient {
        public:
            ChatClient();
            ~ChatClient();
            
            // Connection management
            bool connect(const std::string& server_address, uint16_t port, const std::string& username);
            void disconnect();
            bool isConnected() const { return connected_; }
            
            // User information
            const std::string& getUsername() const { return username_; }
            const std::string& getCurrentRoom() const { return current_room_; }
            const std::string& getServerAddress() const { return server_address_; }
            uint16_t getServerPort() const { return server_port_; }
            
            // Message operations
            void sendMessage(const std::string& content, const std::string& room = constants::DEFAULT_ROOM);
            void sendPrivateMessage(const std::string& recipient, const std::string& content);
            void joinRoom(const std::string& room_name);
            void leaveRoom();
            
            // Room management
            void requestRoomList();
            void requestUserList();
            std::vector<std::string> getAvailableRooms() const;
            std::vector<std::string> getRoomUsers() const;
            
            // User interface integration
            void setUserInterface(std::shared_ptr<UserInterface> ui);
            void runInteractiveMode();
            void runBatchMode(const std::vector<std::string>& commands);
            
            // Statistics and monitoring
            struct ClientStats {
                std::size_t messages_sent;
                std::size_t messages_received;
                std::chrono::steady_clock::time_point connect_time;
                std::chrono::milliseconds average_latency;
                bool is_connected;
            };
            
            ClientStats getStats() const;
            
            // Event callbacks (for custom UI integration)
            using MessageCallback = std::function<void(const Message&)>;
            using ConnectionCallback = std::function<void(bool connected)>;
            using ErrorCallback = std::function<void(const std::string& error)>;
            
            void setMessageCallback(MessageCallback callback) { message_callback_ = callback; }
            void setConnectionCallback(ConnectionCallback callback) { connection_callback_ = callback; }
            void setErrorCallback(ErrorCallback callback) { error_callback_ = callback; }
            
        private:
            // Core networking components
            IoContext io_context_;
            tcp::socket socket_;
            std::atomic<bool> connected_;
            std::thread io_thread_;
            
            // Connection details
            std::string server_address_;
            uint16_t server_port_;
            std::string username_;
            std::string current_room_;
            
            // Message handling
            std::unique_ptr<MessageHandler> message_handler_;
            std::shared_ptr<UserInterface> user_interface_;
            
            // Message queuing for async sending
            std::queue<Message> outgoing_messages_;
            std::mutex outgoing_mutex_;
            std::atomic<bool> write_in_progress_;
            
            // Receive buffer management
            enum { HEADER_LENGTH = 4 };
            std::array<char, HEADER_LENGTH> receive_header_buffer_;
            std::vector<char> receive_body_buffer_;
            
            // Room and user tracking
            mutable std::mutex room_data_mutex_;
            std::vector<std::string> available_rooms_;
            std::vector<std::string> current_room_users_;
            
            // Statistics
            std::atomic<std::size_t> messages_sent_;
            std::atomic<std::size_t> messages_received_;
            std::chrono::steady_clock::time_point connect_time_;
            std::chrono::milliseconds total_latency_;
            
            // Event callbacks
            MessageCallback message_callback_;
            ConnectionCallback connection_callback_;
            ErrorCallback error_callback_;
            
            // Private methods - Connection handling
            void startIoThread();
            void stopIoThread();
            void handleConnectionResult(const error_code& ec);
            
            // Private methods - Message I/O
            void startReceiveHeader();
            void handleReceiveHeader(const error_code& ec, std::size_t bytes_transferred);
            void startReceiveBody(std::uint32_t body_length);
            void handleReceiveBody(const error_code& ec, std::size_t bytes_transferred);
            
            void startSendMessage();
            void handleSendMessage(const error_code& ec, std::size_t bytes_transferred);
            void queueMessage(const Message& message);
            
            // Private methods - Message processing
            void processReceivedMessage(const std::string& message_data);
            void handleIncomingMessage(const Message& message);
            void updateRoomData(const Message& message);
            void notifyCallbacks(const Message& message);
            
            // Private methods - Protocol helpers
            void sendJoinMessage();
            void sendLeaveMessage();
            void sendHeartbeat();
            
            // Private methods - Error handling
            void handleNetworkError(const error_code& ec);
            void cleanup();
        };
        
    } // namespace client
} // namespace chat
