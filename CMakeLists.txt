 
cmake_minimum_required(VERSION 3.20)

# Project configuration
project(RealTimeChatSystem 
    VERSION 1.0.0 
    LANGUAGES CXX
    DESCRIPTION "High-performance C++ chat server and client using Boost.Asio"
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# vcpkg integration check
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

# Find required packages
find_package(Boost REQUIRED COMPONENTS 
    system 
    thread
    serialization
)

# Print Boost information
message(STATUS "Boost version: ${Boost_VERSION}")
message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")

# Include directories
include_directories(${Boost_INCLUDE_DIRS})

# Add subdirectories
add_subdirectory(src)

# Optional: Add tests if enabled
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Optional: Add documentation
option(BUILD_DOCS "Build documentation" OFF)
if(BUILD_DOCS)
    add_subdirectory(docs)
endif()
