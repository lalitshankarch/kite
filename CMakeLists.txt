cmake_minimum_required(VERSION 3.25)
project(kite C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_definitions(-DLUA_USE_POSIX)

include(FetchContent)

FetchContent_Declare(
  SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL/
  GIT_TAG        5d249570393f7a37e037abf22cd6012a4cc56a71 # release-2.32.10
  GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(SDL2)

file(GLOB_RECURSE kite_SOURCES "src/*.c")
add_executable(kite ${kite_SOURCES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(kite PRIVATE m SDL2::SDL2main SDL2::SDL2)
target_compile_options(kite PRIVATE -Wall -Wextra -O3 -fno-strict-aliasing)
set_target_properties(kite PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
