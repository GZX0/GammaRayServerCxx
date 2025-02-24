cmake_minimum_required(VERSION 3.28)
project(GammaRayRelayServer)

set(CMAKE_CXX_STANDARD 20)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS relay_message.proto)

message("Protobuf of tc_message.proto: ${PROTO_SRCS}")

add_library(relay_message
        ${CMAKE_CURRENT_BINARY_DIR}/relay_message.pb.cc
)
target_link_libraries(relay_message libprotobuf)