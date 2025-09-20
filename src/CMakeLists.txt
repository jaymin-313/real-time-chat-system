 
# Source directory CMakeLists.txt
# This file orchestrates the build of all source components

message(STATUS "Configuring source components...")

# Add common library first (other components depend on it)
add_subdirectory(common)

# Add server executable
add_subdirectory(server)

# Add client executable  
add_subdirectory(client)

# Print configuration summary
message(STATUS "Source components configured:")
message(STATUS "  - Common library")
message(STATUS "  - Chat Server")
message(STATUS "  - Chat Client")
