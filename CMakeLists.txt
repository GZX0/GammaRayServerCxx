cmake_minimum_required(VERSION 3.28)

include(env_settings.cmake)
set(CMAKE_PREFIX_PATH ${QT_ROOT}/lib/cmake)
set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

project(GammaRayServer)

include(${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(manager)
add_subdirectory(relay)

add_custom_target(tc_build_all ALL COMMENT "Build all targets")
add_dependencies(tc_build_all GammaRayManageServer GammaRayRelayServer)
