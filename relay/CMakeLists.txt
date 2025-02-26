cmake_minimum_required(VERSION 3.28)
project(GammaRayRelayServer)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(message)
add_subdirectory(settings)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SRCS
        main.cpp
        relay_client.cpp
        relay_client_mgr.cpp
        relay_room.cpp
        relay_room_mgr.cpp
        relay_server.cpp
        relay_session.cpp
        relay_context.cpp
        relay_proto_maker.cpp

        ${SETTINGS_SRCS}
)
add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} tc_common_new relay_message)