cmake_minimum_required(VERSION 3.28)
project(GammaRayRelayServer)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(message)

set(SRCS
        main.cpp
        relay_peer.cpp
        relay_server.cpp
        relay_context.cpp
)
add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} tc_common_new relay_message)