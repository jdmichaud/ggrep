# Usage:
# Create a build folder, cd into it and just do: cmake ../
# Use '-DCMAKE_BUILD_TYPE=debug' to enable -ggdb3 flag.

cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
set (CMAKE_TOOLCHAIN_FILE cmake/clang.cmake)
project(ggrep)

find_package(Threads)
find_package(Curses)
#Find_package(GTest)
find_library(LIBEFENCE_LIBRARY NAMES efence libefence)
find_library(LIBSEGFAULT_LIBRARY NAMES SegFault libSegFault)

add_definitions(-std=c++11)

# Warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -Wfatal-errors -Wno-unused-parameter -Wno-potentially-evaluated-expression")
# Standard libs to use
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
# Optimisation flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Os")
# Activate error level macro
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DS_LOGERR")
# Link option
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
# For debugging
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${LIBEFENCE_LIBRARY} ${LIBSEGFAULT_LIBRARY}")

add_subdirectory(src)
# add_subdirectory(test)
