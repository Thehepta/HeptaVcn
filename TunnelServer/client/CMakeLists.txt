cmake_minimum_required(VERSION 3.23)
project(virtualnet)

set(CMAKE_CXX_STANDARD 14)

#add_executable(virtualnet Tunnel_server.cpp)
#
#target_link_libraries(virtualnet event)




add_executable(virtualnet client/Tunnel_client.cpp)

target_link_libraries(virtualnet event)


set(CMAKE_EXE_LINKER_FLAGS"-static")
