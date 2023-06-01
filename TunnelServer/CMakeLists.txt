cmake_minimum_required(VERSION 3.23)
project(Tunnelserver)

set(CMAKE_CXX_STANDARD 14)


add_executable(Tunnelserver Tunnel_server.cpp)
target_link_libraries(Tunnelserver event)



set(CMAKE_EXE_LINKER_FLAGS"-static")
