cmake_minimum_required(VERSION 3.23)
project(virtualnet)

set(CMAKE_CXX_STANDARD 14)

add_executable(virtualnet udp_server.cpp)

target_link_libraries(virtualnet event)