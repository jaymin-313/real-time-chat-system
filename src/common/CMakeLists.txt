 
# Common library CMakeLists.txt
# Creates a header-only interface library for shared components

message(STATUS "Configuring common library...")

# Create INTERFACE library (header-only)
add_library(chat_common INTERFACE)

# Set include directories for this library
target_include_directories(chat_common 
    INTERFACE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Link required Boost libraries
target_link_libraries(chat_common 
    INTERFACE 
        Boost::system
        Boost::thread
        Boost::serialization
)

# Set compile features
target_compile_features(chat_common 
    INTERFACE 
        cxx_std_20
)

# Add compiler-specific options
target_compile_options(chat_common 
    INTERFACE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# Define preprocessor macros for the library
target_compile_definitions(chat_common
    INTERFACE
        $<$<CONFIG:Debug>:CHAT_DEBUG=1>
        $<$<CONFIG:Release>:CHAT_RELEASE=1>
        BOOST_ASIO_NO_DEPRECATED
)

# Add alias for easier linking
add_library(Chat::Common ALIAS chat_common)

message(STATUS "Common library configured as INTERFACE library")
message(STATUS "  - Include directory: ${CMAKE_CURRENT_SOURCE_DIR}/include")
message(STATUS "  - Boost dependencies: system, thread, serialization")
message(STATUS "  - Alias: Chat::Common")
