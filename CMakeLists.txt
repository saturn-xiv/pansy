cmake_minimum_required(VERSION 3.22)
include(FetchContent)

project(
    pansy
    VERSION 2026.9.14
    DESCRIPTION "Launch a HTTP proxy server over SSH."
    LANGUAGES C CXX)
# --------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# --------------------------------------------------------
execute_process(COMMAND git describe --tags --always --dirty
    OUTPUT_VARIABLE GIT_REV
    ERROR_QUIET
)
string(STRIP "${GIT_REV}" GIT_REV)
string(TIMESTAMP BUILD_TIME UTC)
configure_file(include/pansy/version.h.in ${PROJECT_SOURCE_DIR}/include/pansy/version.hpp)
# --------------------------------------------------------
find_package(argparse CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(boost_exception CONFIG REQUIRED)
# --------------------------------------------------------
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(pansy ${SOURCE_LIST})
set_target_properties(pansy PROPERTIES LINK_FLAGS_RELEASE -s)
target_link_options(pansy PRIVATE -static-libgcc -static-libstdc++)

target_link_libraries(pansy
  Boost::exception
  argparse::argparse
  unofficial-sodium::sodium
)

target_include_directories(pansy PUBLIC
  ${PROJECT_SOURCE_DIR}/include
)
