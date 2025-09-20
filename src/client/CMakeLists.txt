 
# Client executable CMakeLists.txt
# Creates the chat client executable

message(STATUS "Configuring chat client...")

# Collect client source files
set(CLIENT_SOURCES
    main.cpp
)

# Collect client header files (for IDE visibility)
set(CLIENT_HEADERS
    include/client/ChatClient.h
    include/client/MessageHandler.h
    include/client/UserInterface.h
)

# Create client executable
add_executable(chat_client 
    ${CLIENT_SOURCES}
    ${CLIENT_HEADERS}
)

# Set include directories for client
target_include_directories(chat_client 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link dependencies
target_link_libraries(chat_client 
    PRIVATE 
        Chat::Common
        Boost::system
        Boost::thread
        Boost::serialization
)

# Set executable properties
set_target_properties(chat_client PROPERTIES
    OUTPUT_NAME "ChatClient"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Add compiler-specific options
target_compile_options(chat_client 
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Add preprocessor definitions
target_compile_definitions(chat_client
    PRIVATE
        $<$<CONFIG:Debug>:CHAT_CLIENT_DEBUG=1>
        $<$<CONFIG:Release>:CHAT_CLIENT_RELEASE=1>
        BOOST_ASIO_NO_DEPRECATED
)

message(STATUS "Chat client configured:")
message(STATUS "  - Executable name: ChatClient")
message(STATUS "  - Output directory: ${CMAKE_BINARY_DIR}/bin")
message(STATUS "  - Dependencies: Chat::Common, Boost libraries")
