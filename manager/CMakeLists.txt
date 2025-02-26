cmake_minimum_required(VERSION 3.28)
project(GammaRayManageServer)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(message)

set(SRCS
        main.cpp
        mgr_context.cpp
        mgr_server.cpp
        mgr_session.cpp

        ${SETTINGS_SRCS}
)

add_subdirectory(handler)
add_subdirectory(database)
add_subdirectory(settings)

add_executable(${PROJECT_NAME} ${SRCS} ${HANDLER_SRCS} ${DB_SRCS})
target_link_libraries(${PROJECT_NAME} tc_common_new mgr_message sqlite_orm)