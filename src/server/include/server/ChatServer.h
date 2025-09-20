#pragma once

#include "chat/CommonIncludes.h"
#include "ChatSession.h"
#include "ChatRoom.h"
#include <unordered_map>
#include <unordered_set>

namespace chat {
    namespace server {
        
        class ChatServer {
        public:
            explicit ChatServer(IoContext& io_context, uint16_t port);
            ~ChatServer();
            
            // Server lifecycle
            void start();
            void stop();
            bool isRunning() const { return running_; }
            
            // Client management
            void addClient(std::shared_ptr<ChatSession> session);
            void removeClient(std::shared_ptr<ChatSession> session);
            std::size_t getClientCount() const;
            
            // Room management
            std::shared_ptr<ChatRoom> getOrCreateRoom(const std::string& room_name);
            void removeRoom(const std::string& room_name);
            std::vector<std::string> getRoomList() const;
            
            // Message broadcasting
            void broadcastMessage(const Message& message);
            void broadcastToRoom(const Message& message, const std::string& room_name);
            void sendPrivateMessage(const Message& message, const std::string& target_user);
            
            // Statistics
            struct ServerStats {
                std::size_t total_clients;
                std::size_t active_rooms;
                std::size_t messages_sent;
                std::chrono::steady_clock::time_point start_time;
            };
            
            ServerStats getStats() const;
            
        private:
            // Core components
            IoContext& io_context_;
            tcp::acceptor acceptor_;
            std::atomic<bool> running_;
            
            // Client and room management
            std::unordered_set<std::shared_ptr<ChatSession>> clients_;
            std::unordered_map<std::string, std::shared_ptr<ChatRoom>> rooms_;
            std::unordered_map<std::string, std::shared_ptr<ChatSession>> username_to_session_;
            
            // Statistics
            mutable std::mutex stats_mutex_;
            std::atomic<std::size_t> message_count_;
            std::chrono::steady_clock::time_point start_time_;
            
            // Thread safety
            mutable std::shared_mutex clients_mutex_;
            mutable std::shared_mutex rooms_mutex_;
            mutable std::mutex username_mutex_;
            
            // Private methods
            void startAcceptConnections();
            void handleNewConnection(std::shared_ptr<ChatSession> session, const error_code& ec);
            void cleanupEmptyRooms();
            
            // Message handling
            void handleUserJoin(std::shared_ptr<ChatSession> session, const Message& message);
            void handleUserLeave(std::shared_ptr<ChatSession> session, const Message& message);
            void handleChatMessage(std::shared_ptr<ChatSession> session, const Message& message);
            void handlePrivateMessage(std::shared_ptr<ChatSession> session, const Message& message);
            void handleRoomJoin(std::shared_ptr<ChatSession> session, const Message& message);
            void handleRoomLeave(std::shared_ptr<ChatSession> session, const Message& message);
            
            // Utility methods
            bool isUsernameAvailable(const std::string& username) const;
            std::shared_ptr<ChatSession> findSessionByUsername(const std::string& username) const;
            void sendServerAnnouncement(const std::string& message, const std::string& room = "");
            
            // Friend declarations for session access
            friend class ChatSession;
        };
        
    } // namespace server
} // namespace chat
