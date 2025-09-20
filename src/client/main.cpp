#include "chat/CommonIncludes.h"
#include <iostream>
#include <thread>

using namespace chat;
using namespace chat::constants;
using namespace chat::utils;

class ChatClient {
private:
    IoContext io_context_;
    tcp::socket socket_;
    std::string username_;
    std::string server_address_;
    uint16_t server_port_;
    std::atomic<bool> connected_;
    std::thread io_thread_;
    
public:
    ChatClient() : socket_(io_context_), connected_(false) {
        std::cout << "=== Real-Time Chat Client ===" << std::endl;
        std::cout << "Version: 1.0.0" << std::endl;
        std::cout << "=============================" << std::endl;
    }
    
    ~ChatClient() {
        disconnect();
    }
    
    bool connect(const std::string& address, uint16_t port, const std::string& user) {
        try {
            server_address_ = address;
            server_port_ = port;
            username_ = user;
            
            std::cout << "Connecting to " << address << ":" << port << "..." << std::endl;
            CHAT_LOG("Attempting connection to " << address << ":" << port);
            
            // Resolve server address
            tcp::resolver resolver(io_context_);
            auto endpoints = resolver.resolve(address, std::to_string(port));
            
            // Connect to server
            boost::asio::connect(socket_, endpoints);
            
            connected_ = true;
            std::cout << "Connected to server!" << std::endl;
            CHAT_LOG("Successfully connected to server");
            
            // Start IO thread for receiving messages
            io_thread_ = std::thread([this]() {
                try {
                    io_context_.run();
                } catch (const std::exception& e) {
                    CHAT_ERROR("IO thread error: " << e.what());
                }
            });
            
            // Send join message
            sendJoinMessage();
            
            // Start receiving messages
            startReceive();
            
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Connection failed: " << e.what() << std::endl;
            CHAT_ERROR("Connection error: " << e.what());
            connected_ = false;
            return false;
        }
    }
    
    void disconnect() {
        if (!connected_) return;
        
        try {
            // Send leave message
            sendLeaveMessage();
            
            connected_ = false;
            socket_.close();
            io_context_.stop();
            
            if (io_thread_.joinable()) {
                io_thread_.join();
            }
            
            std::cout << "Disconnected from server." << std::endl;
            CHAT_LOG("Client disconnected");
            
        } catch (const std::exception& e) {
            CHAT_ERROR("Disconnect error: " << e.what());
        }
    }
    
    void sendMessage(const std::string& content, const std::string& room = DEFAULT_ROOM) {
        if (!connected_) {
            std::cout << "Not connected to server!" << std::endl;
            return;
        }
        
        try {
            Message msg(MessageType::CHAT_MESSAGE, username_, content, room);
            std::string serialized = serializeMessage(msg);
            
            // Send message size first
            uint32_t msg_size = static_cast<uint32_t>(serialized.size());
            boost::asio::write(socket_, boost::asio::buffer(&msg_size, sizeof(msg_size)));
            
            // Send the message
            boost::asio::write(socket_, boost::asio::buffer(serialized));
            
            CHAT_LOG("Message sent: " << content);
            
        } catch (const std::exception& e) {
            std::cerr << "Failed to send message: " << e.what() << std::endl;
            CHAT_ERROR("Send error: " << e.what());
        }
    }
    
    void runConsoleInterface() {
        if (!connected_) {
            std::cout << "Not connected to server!" << std::endl;
            return;
        }
        
        std::cout << "\n=== Chat Commands ===" << std::endl;
        std::cout << "/quit - Exit the chat" << std::endl;
        std::cout << "/help - Show this help" << std::endl;
        std::cout << "Just type your message and press Enter" << std::endl;
        std::cout << "===================" << std::endl;
        std::cout << "You are now in the chat as '" << username_ << "'" << std::endl;
        std::cout << "Type your messages below:" << std::endl;
        
        std::string input;
        while (connected_ && std::getline(std::cin, input)) {
            if (input.empty()) continue;
            
            // Handle commands
            if (input == "/quit") {
                std::cout << "Goodbye!" << std::endl;
                break;
            } else if (input == "/help") {
                std::cout << "\n=== Available Commands ===" << std::endl;
                std::cout << "/quit - Exit the chat" << std::endl;
                std::cout << "/help - Show this help" << std::endl;
                std::cout << "========================" << std::endl;
                continue;
            }
            
            // Send regular message
            sendMessage(input);
        }
    }
    
private:
    void sendJoinMessage() {
        try {
            Message join_msg(MessageType::USER_JOIN, username_, 
                           username_ + " has joined the chat", DEFAULT_ROOM);
            
            std::string serialized = serializeMessage(join_msg);
            uint32_t msg_size = static_cast<uint32_t>(serialized.size());
            
            boost::asio::write(socket_, boost::asio::buffer(&msg_size, sizeof(msg_size)));
            boost::asio::write(socket_, boost::asio::buffer(serialized));
            
            CHAT_LOG("Join message sent");
            
        } catch (const std::exception& e) {
            CHAT_ERROR("Failed to send join message: " << e.what());
        }
    }
    
    void sendLeaveMessage() {
        try {
            Message leave_msg(MessageType::USER_LEAVE, username_, 
                            username_ + " has left the chat", DEFAULT_ROOM);
            
            std::string serialized = serializeMessage(leave_msg);
            uint32_t msg_size = static_cast<uint32_t>(serialized.size());
            
            boost::asio::write(socket_, boost::asio::buffer(&msg_size, sizeof(msg_size)));
            boost::asio::write(socket_, boost::asio::buffer(serialized));
            
            CHAT_LOG("Leave message sent");
            
        } catch (const std::exception& e) {
            CHAT_ERROR("Failed to send leave message: " << e.what());
        }
    }
    
    void startReceive() {
        // For now, implement basic message receiving
        // In next steps, we'll make this fully asynchronous
        auto size_buffer = std::make_shared<uint32_t>();
        
        boost::asio::async_read(socket_, 
            boost::asio::buffer(size_buffer.get(), sizeof(uint32_t)),
            [this, size_buffer](error_code ec, std::size_t /*bytes_transferred*/) {
                if (!ec && connected_) {
                    receiveMessage(*size_buffer);
                } else if (ec) {
                    CHAT_ERROR("Receive size error: " << ec.message());
                    connected_ = false;
                }
            });
    }
    
    void receiveMessage(uint32_t message_size) {
        auto message_buffer = std::make_shared<std::vector<char>>(message_size);
        
        boost::asio::async_read(socket_,
            boost::asio::buffer(*message_buffer),
            [this, message_buffer](error_code ec, std::size_t /*bytes_transferred*/) {
                if (!ec && connected_) {
                    try {
                        std::string message_data(message_buffer->begin(), message_buffer->end());
                        Message received_msg = deserializeMessage(message_data);
                        
                        // Display the message
                        std::cout << formatMessage(received_msg) << std::endl;
                        
                        // Continue receiving
                        startReceive();
                        
                    } catch (const std::exception& e) {
                        CHAT_ERROR("Message processing error: " << e.what());
                    }
                } else if (ec) {
                    CHAT_ERROR("Receive message error: " << ec.message());
                    connected_ = false;
                }
            });
    }
};

int main(int argc, char* argv[]) {
    try {
        std::string server_address = "localhost";
        uint16_t server_port = DEFAULT_PORT;
        std::string username;
        
        // Parse command line arguments
        if (argc > 1) {
            server_address = argv[1];
        }
        if (argc > 2) {
            server_port = static_cast<uint16_t>(std::stoi(argv[2]));
        }
        if (argc > 3) {
            username = argv[3];
        }
        
        // Get username if not provided
        if (username.empty()) {
            std::cout << "Enter your username: ";
            std::getline(std::cin, username);
            
            if (username.empty()) {
                std::cerr << "Username cannot be empty!" << std::endl;
                return 1;
            }
        }
        
        // Create and connect client
        ChatClient client;
        
        if (!client.connect(server_address, server_port, username)) {
            std::cerr << "Failed to connect to server." << std::endl;
            return 1;
        }
        
        // Run console interface
        client.runConsoleInterface();
        
        // Clean disconnect
        client.disconnect();
        
    } catch (const std::exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
