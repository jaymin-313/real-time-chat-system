 
#include "chat/CommonIncludes.h"
#include <iostream>
#include <signal.h>

using namespace chat;
using namespace chat::constants;
using namespace chat::utils;

class ChatServer {
private:
    IoContext io_context_;
    tcp::acceptor acceptor_;
    std::atomic<bool> running_;
    
public:
    explicit ChatServer(uint16_t port) 
        : acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)), 
          running_(true) {
        
        std::cout << "ChatServer starting on port " << port << std::endl;
        CHAT_LOG("Server initialized with acceptor on port " << port);
    }
    
    void start() {
        std::cout << "Server is running. Press Ctrl+C to stop." << std::endl;
        
        // Start accepting connections
        startAccept();
        
        // Run the IO context
        try {
            io_context_.run();
        } catch (const std::exception& e) {
            CHAT_ERROR("Server error: " << e.what());
            std::cerr << "Server error: " << e.what() << std::endl;
        }
    }
    
    void stop() {
        running_ = false;
        io_context_.stop();
        std::cout << "\nServer shutting down..." << std::endl;
    }
    
private:
    void startAccept() {
        if (!running_) return;
        
        // For now, we'll create a basic connection handler
        auto socket = std::make_shared<tcp::socket>(io_context_);
        
        acceptor_.async_accept(*socket,
            [this, socket](error_code ec) {
                if (!ec && running_) {
                    handleAccept(socket);
                    startAccept(); // Continue accepting new connections
                } else if (ec) {
                    CHAT_ERROR("Accept error: " << ec.message());
                }
            });
    }
    
    void handleAccept(std::shared_ptr<tcp::socket> socket) {
        try {
            auto endpoint = socket->remote_endpoint();
            std::cout << "New connection from: " << endpoint.address().to_string() 
                      << ":" << endpoint.port() << std::endl;
            
            CHAT_LOG("Client connected from " << endpoint.address().to_string());
            
            // Send welcome message
            Message welcome_msg(MessageType::SERVER_ANNOUNCEMENT, SERVER_NAME, 
                               "Welcome to the Chat Server!", DEFAULT_ROOM);
            
            std::string serialized = serializeMessage(welcome_msg);
            
            // Send message size first (simple protocol)
            uint32_t msg_size = static_cast<uint32_t>(serialized.size());
            boost::asio::write(*socket, boost::asio::buffer(&msg_size, sizeof(msg_size)));
            
            // Send the actual message
            boost::asio::write(*socket, boost::asio::buffer(serialized));
            
            std::cout << "Welcome message sent to new client" << std::endl;
            
            // For now, close the connection after sending welcome message
            // In future steps, we'll keep connections alive and handle messaging
            socket->close();
            
        } catch (const std::exception& e) {
            CHAT_ERROR("Error handling new connection: " << e.what());
            std::cerr << "Error handling connection: " << e.what() << std::endl;
        }
    }
};

// Global server instance for signal handling
std::unique_ptr<ChatServer> g_server;

void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << std::endl;
    if (g_server) {
        g_server->stop();
    }
}

int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments
        uint16_t port = DEFAULT_PORT;
        if (argc > 1) {
            port = static_cast<uint16_t>(std::stoi(argv[1]));
        }
        
        std::cout << "=== Real-Time Chat Server ===" << std::endl;
        std::cout << "Version: 1.0.0" << std::endl;
        std::cout << "Port: " << port << std::endl;
        std::cout << "Max clients: " << MAX_CLIENTS << std::endl;
        std::cout << "==============================" << std::endl;
        
        // Setup signal handlers for graceful shutdown
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);
        
        // Create and start server
        g_server = std::make_unique<ChatServer>(port);
        g_server->start();
        
        std::cout << "Server stopped successfully." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
