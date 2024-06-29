cmake_minimum_required(VERSION 3.0)
project(CPPServer)

set(CMAKE_CXX_STANDARD 11)

add_executable(server server.cc)
add_executable(client client.cc)