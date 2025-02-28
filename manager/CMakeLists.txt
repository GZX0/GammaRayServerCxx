cmake_minimum_required(VERSION 3.28)
project(GammaRayManageServer)

set(CMAKE_CXX_STANDARD 20)

find_package(bsoncxx CONFIG REQUIRED)
find_package(mongocxx CONFIG REQUIRED)

add_subdirectory(settings)
add_subdirectory(handler)
add_subdirectory(database)

set(SRCS
        main.cpp
        mgr_context.cpp
        mgr_server.cpp
        mgr_session.cpp

        ${SETTINGS_SRCS}
        ${HANDLER_SRCS}
        ${DB_SRCS}
)

add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} tc_common_new tc_mgr_message)
target_link_libraries(${PROJECT_NAME} $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>)
target_link_libraries(${PROJECT_NAME} $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>)