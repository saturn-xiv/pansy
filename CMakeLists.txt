cmake_minimum_required(VERSION 3.22)
include(FetchContent)

project(
    pansy
    VERSION 2026.9.15
    DESCRIPTION "Launch a HTTP proxy server over SSH."
    LANGUAGES C CXX)
# --------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# --------------------------------------------------------
add_subdirectory(begonia)
add_subdirectory(tests)
# --------------------------------------------------------
add_executable(pansy ${PROJECT_SOURCE_DIR}/srv/main.cpp)
set_target_properties(pansy PROPERTIES LINK_FLAGS_RELEASE -s)
target_link_options(pansy PRIVATE -static-libgcc -static-libstdc++ -Wl,--build-id=sha1)
target_link_libraries(pansy begonia)
