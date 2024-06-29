cmake_minimum_required(VERSION 3.0)
project(CPPServer)

set(CMAKE_CXX_STANDARD 11)

add_executable(server server.cpp util.cpp)
add_executable(client client.cpp util.cpp)

add_custom_target( cln
    COMMAND ${CMAKE_COMMAND} -E remove server client
    COMMENT "Clean"
)