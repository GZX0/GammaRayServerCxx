cmake_minimum_required(VERSION 3.28)

include(env_settings.cmake)
set(CMAKE_PREFIX_PATH ${QT_ROOT}/lib/cmake)
set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

project(GammaRayServer)
set(CMAKE_CXX_STANDARD 20)
include(${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

include_directories(${CMAKE_SOURCE_DIR}/deps)
include_directories(${CMAKE_SOURCE_DIR}/deps/tc_3rdparty)
include_directories(${CMAKE_SOURCE_DIR}/deps/tc_3rdparty/eventbus/src)
include_directories(${CMAKE_BINARY_DIR}/deps/tc_message_new)
include_directories(${CMAKE_SOURCE_DIR}/deps/tc_3rdparty/asio2/include)
include_directories(${CMAKE_SOURCE_DIR}/deps/tc_3rdparty/asio2/3rd)
include_directories(${CMAKE_SOURCE_DIR}/deps/tc_3rdparty/asio2/3rd/asio)

if(MSVC)
    add_compile_options($<IF:$<CONFIG:Debug>,/MTd,/MT>)
    add_compile_options("/bigobj")
    include_directories(${CMAKE_SOURCE_DIR}/deps/tc_3rdparty/openssl/include)
    link_directories(${CMAKE_SOURCE_DIR}/deps/tc_3rdparty/openssl/x64/lib/VC)
endif()

set(Protobuf_SRC_ROOT_FOLDER ${CMAKE_SOURCE_DIR}/deps/tc_3rdparty/protobuf)
if(CMAKE_BUILD_TYPE MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    set(Protobuf_LIBRARIES ${CMAKE_BINARY_DIR}/deps/tc_3rdparty/protobuf/cmake/libprotobuf.lib)
endif()
find_package(Protobuf REQUIRED)

add_subdirectory(deps)
add_subdirectory(manager)
add_subdirectory(relay)

add_custom_target(tc_build_all ALL COMMENT "Build all targets")
add_dependencies(tc_build_all GammaRayManageServer GammaRayRelayServer)
