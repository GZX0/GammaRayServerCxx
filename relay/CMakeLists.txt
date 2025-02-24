cmake_minimum_required(VERSION 3.28)
project(GammaRayRelayServer)

set(CMAKE_CXX_STANDARD 20)

set(SRCS
        main.cpp
        relay_peer.cpp
        relay_server.cpp
)
add_executable(${PROJECT_NAME} ${SRCS})