#pragma once

#include "chat/CommonIncludes.h"
#include <memory>
#include <thread>
#include <atomic>

namespace chat {
    namespace client {
        
        // Forward declaration
        class ChatClient;
        
        class UserInterface {
        public:
            explicit UserInterface(ChatClient& client);
            virtual ~UserInterface() = default;
            
            // Interface lifecycle
            virtual void start() = 0;
            virtual void stop() = 0;
            virtual bool isRunning() const = 0;
            
            // Message display
            virtual void displayMessage(const std::string& message) = 0;
            virtual void displayError(const std::string& error) = 0;
            virtual void displayStatus(const std::string& status) = 0;
            
            // User input
            virtual std::string getUserInput(const std::string& prompt = "") = 0;
            virtual bool confirmAction(const std::string& question) = 0;
            
        protected:
            ChatClient& client_;
        };
        
        // Console-based implementation
        class ConsoleInterface : public UserInterface {
        public:
            explicit ConsoleInterface(ChatClient& client);
            ~ConsoleInterface() override;
            
            // UserInterface implementation
            void start() override;
            void stop() override;
            bool isRunning() const override { return running_; }
            
            void displayMessage(const std::string& message) override;
            void displayError(const std::string& error) override;
            void displayStatus(const std::string& status) override;
            
            std::string getUserInput(const std::string& prompt = "") override;
            bool confirmAction(const std::string& question) override;
            
            // Console-specific features
            void showWelcomeScreen();
            void showHelpScreen();
            void showConnectionInfo();
            void showRoomInfo();
            void showUserStats();
            
            // Command handling
            struct Command {
                std::string name;
                std::string description;
                std::function<void(const std::vector<std::string>&)> handler;
                std::size_t min_args;
                std::size_t max_args;
            };
            
            void registerCommand(const Command& command);
            void unregisterCommand(const std::string& name);
            bool executeCommand(const std::string& input);
            void showAvailableCommands();
            
        private:
            std::atomic<bool> running_;
            std::thread input_thread_;
            
            // Command system
            std::unordered_map<std::string, Command> commands_;
            mutable std::mutex commands_mutex_;
            
            // Display formatting
            struct DisplaySettings {
                bool show_colors = true;
                bool show_timestamps = true;
                std::string prompt = "> ";
                std::size_t max_line_length = 80;
            } display_settings_;
            
            // Private methods
            void runInputLoop();
            void initializeCommands();
            void handleUserInput(const std::string& input);
            std::vector<std::string> parseCommand(const std::string& input);
            
            // Built-in command handlers
            void handleHelpCommand(const std::vector<std::string>& args);
            void handleQuitCommand(const std::vector<std::string>& args);
            void handleJoinCommand(const std::vector<std::string>& args);
            void handleLeaveCommand(const std::vector<std::string>& args);
            void handlePrivateCommand(const std::vector<std::string>& args);
            void handleRoomsCommand(const std::vector<std::string>& args);
            void handleUsersCommand(const std::vector<std::string>& args);
            void handleStatsCommand(const std::vector<std::string>& args);
            void handleClearCommand(const std::vector<std::string>& args);
            
            // Utility methods
            void printFormatted(const std::string& message, const std::string& prefix = "");
            void printColored(const std::string& message, const std::string& color);
            void clearScreen();
            std::string getCurrentTimeString() const;
            
            // Console colors
            static const std::string RESET;
            static const std::string RED;
            static const std::string GREEN;
            static const std::string BLUE;
            static const std::string YELLOW;
            static const std::string MAGENTA;
            static const std::string CYAN;
            static const std::string WHITE;
        };
        
    } // namespace client
} // namespace chat
