cmake_minimum_required(VERSION 3.28)
project(GammaRayManageServer)

set(CMAKE_CXX_STANDARD 20)
set(SRCS
        main.cpp
)
add_executable(${PROJECT_NAME} ${SRCS})