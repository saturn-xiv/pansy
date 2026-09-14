set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(target x86_64-w64-mingw32)

set(CMAKE_C_COMPILER /usr/bin/${target}-gcc)
set(CMAKE_C_COMPILER_TARGET ${target})
set(CMAKE_CXX_COMPILER /usr/bin/${target}-g++)
set(CMAKE_CXX_COMPILER_TARGET ${target})
