set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

set(target riscv64-linux-gnu)

set(CMAKE_C_COMPILER /usr/bin/${target}-gcc)
set(CMAKE_C_COMPILER_TARGET ${target})
set(CMAKE_CXX_COMPILER /usr/bin/${target}-g++)
set(CMAKE_CXX_COMPILER_TARGET ${target})
