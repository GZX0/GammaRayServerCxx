cmake_minimum_required(VERSION 3.28)
project(GammaRayRelayServer)

set(CMAKE_CXX_STANDARD 20)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS mgr_message.proto)

message("Protobuf of tc_message.proto: ${PROTO_SRCS}")

add_library(mgr_message
        ${CMAKE_CURRENT_BINARY_DIR}/mgr_message.pb.cc
)
target_link_libraries(mgr_message libprotobuf)