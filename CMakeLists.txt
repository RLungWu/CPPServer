cmake_minimum_required(VERSION 3.10)
project(CPPServer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the include directory
include_directories(include)

# Add executable for the client
add_executable(client
    src/util.cpp
    src/client.cpp
)

# Add executable for the server
add_executable(server
    src/util.cpp
    src/server.cpp
    src/epoll.cpp
    src/inet_address.cpp
    src/socket.cpp
)

# Add custom target for cleaning
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove server client
)