cmake_minimum_required(VERSION 3.1)

project(cppevent
    VERSION 1.0
    DESCRIPTION "cppevent library"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

add_subdirectory(libs/doctest)
add_subdirectory(libs/cppevent-base)
add_subdirectory(libs/cppevent-net)
add_subdirectory(libs/cppevent-fcgi)

add_subdirectory(examples/timer)
add_subdirectory(examples/echo)
add_subdirectory(examples/telnet)
