 
# Tests CMakeLists.txt
# Configures unit tests for the chat system

message(STATUS "Configuring tests...")

# Check if testing is enabled
if(NOT BUILD_TESTS)
    message(STATUS "Tests disabled, skipping test configuration")
    return()
endif()

# Find or fetch GoogleTest
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    message(STATUS "GoogleTest not found, fetching from GitHub...")
    
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    
    # For Windows: Prevent overriding parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    # Create aliases for consistent naming
    add_library(GTest::GTest ALIAS gtest)
    add_library(GTest::Main ALIAS gtest_main)
endif()

# Test source files
set(TEST_SOURCES
    test_protocol.cpp
    test_message_serialization.cpp
    test_chat_room.cpp
    test_chat_session.cpp
)

# Create test executable
add_executable(chat_tests ${TEST_SOURCES})

# Set include directories
target_include_directories(chat_tests 
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/common/include
        ${CMAKE_SOURCE_DIR}/src/server/include  
        ${CMAKE_SOURCE_DIR}/src/client/include
)

# Link test dependencies
target_link_libraries(chat_tests 
    PRIVATE
        Chat::Common
        GTest::GTest
        GTest::Main
        Boost::system
        Boost::thread
        Boost::serialization
)

# Set test properties
set_target_properties(chat_tests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
)

# Add compiler-specific options
target_compile_options(chat_tests 
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Add preprocessor definitions
target_compile_definitions(chat_tests
    PRIVATE
        CHAT_TESTING=1
        BOOST_ASIO_NO_DEPRECATED
)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(chat_tests)

# Add individual test cases
add_test(NAME ChatProtocolTests COMMAND chat_tests --gtest_filter="ProtocolTest*")
add_test(NAME MessageSerializationTests COMMAND chat_tests --gtest_filter="SerializationTest*") 
add_test(NAME ChatRoomTests COMMAND chat_tests --gtest_filter="ChatRoomTest*")
add_test(NAME ChatSessionTests COMMAND chat_tests --gtest_filter="ChatSessionTest*")

message(STATUS "Tests configured:")
message(STATUS "  - GoogleTest integration: ${GTest_FOUND}")
message(STATUS "  - Test executable: chat_tests")
message(STATUS "  - Output directory: ${CMAKE_BINARY_DIR}/bin/tests")
message(STATUS "  - Individual test suites registered with CTest")
