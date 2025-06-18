cmake_minimum_required(VERSION 3.25)
project(kite C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_definitions(-DLUA_USE_POSIX)

file(GLOB_RECURSE kite_SOURCES "src/*.c")
add_executable(kite ${kite_SOURCES})

find_package(SDL2 REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(kite PRIVATE m SDL2)
target_compile_options(kite PRIVATE -Wall -Wextra -O3 -fno-strict-aliasing)
set_target_properties(kite PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
