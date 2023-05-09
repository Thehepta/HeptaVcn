cmake_minimum_required(VERSION 3.23)
project(client)

set(CMAKE_CXX_STANDARD 14)


add_executable(client Tunnel_client.cpp)

target_link_libraries(client event)


set(CMAKE_EXE_LINKER_FLAGS"-static")
