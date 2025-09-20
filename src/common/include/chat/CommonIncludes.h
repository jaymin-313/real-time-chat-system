#pragma once

// Standard C++ includes
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <functional>
#include <algorithm>

// Boost.Asio includes for networking
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/system/error_code.hpp>

// Boost.Serialization includes
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

// Boost.Thread includes
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

// Namespace aliases for cleaner code
namespace asio = boost::asio;
namespace ip = boost::asio::ip;
using tcp = boost::asio::ip::tcp;
using error_code = boost::system::error_code;

// Common type aliases
using MessageBuffer = std::vector<char>;
using ConnectionPtr = std::shared_ptr<tcp::socket>;
using IoContext = boost::asio::io_context;

// Chat system includes
#include "Protocol.h"

// Common constants
namespace chat {
    namespace constants {
        // Network configuration
        static constexpr uint16_t DEFAULT_PORT = 8080;
        static constexpr size_t MAX_MESSAGE_LENGTH = 4096;
        static constexpr size_t HEADER_LENGTH = 8;  // Size of message length header
        static constexpr uint32_t MAX_CLIENTS = 1000;
        
        // Timing configuration
        static constexpr std::chrono::seconds HEARTBEAT_INTERVAL{30};
        static constexpr std::chrono::seconds CONNECTION_TIMEOUT{60};
        
        // Default values
        static const std::string DEFAULT_ROOM = "lobby";
        static const std::string SERVER_NAME = "ChatServer";
        
        // Message formatting
        static const std::string MESSAGE_DELIMITER = "\n";
        static const std::string TIMESTAMP_FORMAT = "[%H:%M:%S]";
    }
    
    // Utility functions
    namespace utils {
        // Get current timestamp as string
        inline std::string getCurrentTimestamp() {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), constants::TIMESTAMP_FORMAT.c_str());
            return ss.str();
        }
        
        // Serialize message to string
        inline std::string serializeMessage(const Message& msg) {
            std::ostringstream oss;
            boost::archive::text_oarchive oa(oss);
            oa << msg;
            return oss.str();
        }
        
        // Deserialize message from string
        inline Message deserializeMessage(const std::string& data) {
            Message msg;
            std::istringstream iss(data);
            boost::archive::text_iarchive ia(iss);
            ia >> msg;
            return msg;
        }
        
        // Format message for display
        inline std::string formatMessage(const Message& msg) {
            std::stringstream ss;
            ss << getCurrentTimestamp() << " ";
            
            switch (msg.type) {
                case MessageType::CHAT_MESSAGE:
                    if (!msg.room.empty() && msg.room != constants::DEFAULT_ROOM) {
                        ss << "[" << msg.room << "] ";
                    }
                    ss << msg.sender << ": " << msg.content;
                    break;
                    
                case MessageType::PRIVATE_MESSAGE:
                    ss << "[Private] " << msg.sender << " -> " << msg.recipient << ": " << msg.content;
                    break;
                    
                case MessageType::USER_JOIN:
                    ss << "*** " << msg.sender << " joined the chat";
                    if (!msg.room.empty()) ss << " in room " << msg.room;
                    break;
                    
                case MessageType::USER_LEAVE:
                    ss << "*** " << msg.sender << " left the chat";
                    break;
                    
                case MessageType::SERVER_ANNOUNCEMENT:
                    ss << "[SERVER] " << msg.content;
                    break;
                    
                default:
                    ss << msg.content;
                    break;
            }
            
            return ss.str();
        }
    }
}

// Debug macros (only in debug builds)
#ifdef CHAT_DEBUG
    #define CHAT_LOG(msg) std::cout << "[DEBUG] " << msg << std::endl
    #define CHAT_ERROR(msg) std::cerr << "[ERROR] " << msg << std::endl
#else
    #define CHAT_LOG(msg)
    #define CHAT_ERROR(msg)
#endif
