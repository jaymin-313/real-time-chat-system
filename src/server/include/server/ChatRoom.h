#pragma once

#include "chat/CommonIncludes.h"
#include <unordered_set>
#include <shared_mutex>

namespace chat {
    namespace server {
        
        // Forward declaration
        class ChatSession;
        
        class ChatRoom {
        public:
            explicit ChatRoom(const std::string& name, std::uint32_t max_users = 100, bool is_private = false);
            ~ChatRoom() = default;
            
            // Room information
            const std::string& getName() const { return name_; }
            std::uint32_t getMaxUsers() const { return max_users_; }
            bool isPrivate() const { return is_private_; }
            std::size_t getUserCount() const;
            bool isEmpty() const;
            bool isFull() const;
            
            // User management
            bool addUser(std::shared_ptr<ChatSession> session);
            bool removeUser(std::shared_ptr<ChatSession> session);
            bool hasUser(std::shared_ptr<ChatSession> session) const;
            bool hasUser(const std::string& username) const;
            
            // Get user list
            std::vector<std::string> getUserList() const;
            std::vector<std::shared_ptr<ChatSession>> getActiveSessions() const;
            
            // Message broadcasting
            void broadcastMessage(const Message& message, std::shared_ptr<ChatSession> exclude = nullptr);
            void sendUserList(std::shared_ptr<ChatSession> session);
            
            // Room settings
            void setMaxUsers(std::uint32_t max_users);
            void setPrivate(bool is_private);
            
            // Statistics
            struct RoomStats {
                std::size_t current_users;
                std::size_t max_users;
                std::size_t total_messages;
                std::chrono::steady_clock::time_point created_time;
                bool is_private;
            };
            
            RoomStats getStats() const;
            
        private:
            // Room properties
            std::string name_;
            std::uint32_t max_users_;
            std::atomic<bool> is_private_;
            std::chrono::steady_clock::time_point created_time_;
            
            // User management
            std::unordered_set<std::shared_ptr<ChatSession>> users_;
            mutable std::shared_mutex users_mutex_;
            
            // Statistics
            std::atomic<std::size_t> total_messages_;
            
            // Helper methods
            void sendJoinNotification(std::shared_ptr<ChatSession> new_user);
            void sendLeaveNotification(const std::string& username);
            void cleanupInactiveSessions();
        };
        
    } // namespace server
} // namespace chat
