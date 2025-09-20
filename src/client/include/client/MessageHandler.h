#pragma once

#include "chat/CommonIncludes.h"
#include <functional>
#include <unordered_map>

namespace chat {
    namespace client {
        
        // Forward declaration
        class ChatClient;
        
        class MessageHandler {
        public:
            explicit MessageHandler(ChatClient& client);
            ~MessageHandler() = default;
            
            // Message processing
            void processMessage(const Message& message);
            void formatAndDisplay(const Message& message);
            
            // Message type handlers
            void handleChatMessage(const Message& message);
            void handlePrivateMessage(const Message& message);
            void handleUserJoin(const Message& message);
            void handleUserLeave(const Message& message);
            void handleRoomJoin(const Message& message);
            void handleRoomLeave(const Message& message);
            void handleServerAnnouncement(const Message& message);
            void handleHeartbeat(const Message& message);
            
            // Message formatting
            std::string formatChatMessage(const Message& message) const;
            std::string formatPrivateMessage(const Message& message) const;
            std::string formatSystemMessage(const Message& message) const;
            std::string formatTimestamp(uint64_t timestamp) const;
            
            // Display options
            struct DisplayOptions {
                bool show_timestamps = true;
                bool show_room_names = true;
                bool show_join_leave = true;
                bool colorize_output = false;
                std::string timestamp_format = "[%H:%M:%S]";
            };
            
            void setDisplayOptions(const DisplayOptions& options) { display_options_ = options; }
            const DisplayOptions& getDisplayOptions() const { return display_options_; }
            
            // Message filtering
            using MessageFilter = std::function<bool(const Message&)>;
            void addMessageFilter(const std::string& name, MessageFilter filter);
            void removeMessageFilter(const std::string& name);
            void clearMessageFilters();
            
            // Message history
            void enableHistory(std::size_t max_messages = 1000);
            void disableHistory();
            std::vector<Message> getMessageHistory() const;
            void clearHistory();
            
        private:
            ChatClient& client_;
            DisplayOptions display_options_;
            
            // Message type dispatch table
            using MessageHandlerFunc = std::function<void(const Message&)>;
            std::unordered_map<MessageType, MessageHandlerFunc> message_handlers_;
            
            // Message filtering
            std::unordered_map<std::string, MessageFilter> message_filters_;
            
            // Message history
            bool history_enabled_;
            std::size_t max_history_size_;
            mutable std::mutex history_mutex_;
            std::deque<Message> message_history_;
            
            // Private methods
            void initializeMessageHandlers();
            bool passesFilters(const Message& message) const;
            void addToHistory(const Message& message);
            std::string colorizeMessage(const std::string& message, MessageType type) const;
            
            // Console color codes (for terminals that support them)
            static const std::string COLOR_RESET;
            static const std::string COLOR_RED;
            static const std::string COLOR_GREEN;
            static const std::string COLOR_BLUE;
            static const std::string COLOR_YELLOW;
            static const std::string COLOR_MAGENTA;
            static const std::string COLOR_CYAN;
        };
        
    } // namespace client
} // namespace chat
