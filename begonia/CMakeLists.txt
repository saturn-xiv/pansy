find_package(argparse CONFIG REQUIRED)
find_package(tomlplusplus CONFIG REQUIRED)
find_package(libssh2 CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED COMPONENTS asio)
find_package(Boost CONFIG REQUIRED COMPONENTS beast)
find_package(Boost CONFIG REQUIRED COMPONENTS serialization)
find_package(Boost CONFIG REQUIRED COMPONENTS iostreams)
find_package(Boost CONFIG REQUIRED COMPONENTS log)
find_package(Boost CONFIG REQUIRED COMPONENTS exception)

find_path(CPPCODEC_INCLUDE_DIRS "cppcodec/base32_crockford.hpp")
# -----------------------------------------------------------------------------
execute_process(COMMAND git describe --tags --always --dirty
    OUTPUT_VARIABLE GIT_REV
    ERROR_QUIET
)
string(STRIP "${GIT_REV}" GIT_REV)
string(TIMESTAMP BUILD_TIME UTC)
configure_file(include/pansy/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/pansy/version.hpp)
# -----------------------------------------------------------------------------
file(GLOB BEGONIA_SOURCE_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(begonia ${BEGONIA_SOURCE_LIST})
target_link_libraries(begonia
  Boost::asio Boost::beast Boost::serialization Boost::iostreams Boost::log Boost::exception
  argparse::argparse tomlplusplus::tomlplusplus libssh2::libssh2 unofficial-sodium::sodium
)

target_include_directories(begonia PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CPPCODEC_INCLUDE_DIRS}
)
