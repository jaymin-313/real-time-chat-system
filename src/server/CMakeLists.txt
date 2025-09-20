 
# Server executable CMakeLists.txt
# Creates the chat server executable

message(STATUS "Configuring chat server...")

# Collect server source files
set(SERVER_SOURCES
    main.cpp
)

# Collect server header files (for IDE visibility)
set(SERVER_HEADERS
    include/server/ChatServer.h
    include/server/ChatSession.h
    include/server/ChatRoom.h
)

# Create server executable
add_executable(chat_server 
    ${SERVER_SOURCES}
    ${SERVER_HEADERS}
)

# Set include directories for server
target_include_directories(chat_server 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link dependencies
target_link_libraries(chat_server 
    PRIVATE 
        Chat::Common
        Boost::system
        Boost::thread
        Boost::serialization
)

# Set executable properties
set_target_properties(chat_server PROPERTIES
    OUTPUT_NAME "ChatServer"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Add compiler-specific options
target_compile_options(chat_server 
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Add preprocessor definitions
target_compile_definitions(chat_server
    PRIVATE
        $<$<CONFIG:Debug>:CHAT_SERVER_DEBUG=1>
        $<$<CONFIG:Release>:CHAT_SERVER_RELEASE=1>
        BOOST_ASIO_NO_DEPRECATED
)

message(STATUS "Chat server configured:")
message(STATUS "  - Executable name: ChatServer")
message(STATUS "  - Output directory: ${CMAKE_BINARY_DIR}/bin")
message(STATUS "  - Dependencies: Chat::Common, Boost libraries")
