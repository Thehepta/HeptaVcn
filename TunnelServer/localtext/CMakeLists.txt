cmake_minimum_required(VERSION 3.23)
project(localtext)

set(CMAKE_CXX_STANDARD 14)


add_executable(localtext Tunnel_client.cpp)


set(CMAKE_EXE_LINKER_FLAGS"-static")
