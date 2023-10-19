cmake_minimum_required(VERSION 3.5)

project(cppevent
    VERSION 1.0
    DESCRIPTION "cppevent library"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

option(CPPEVENT_TEST "Build test files" OFF)
option(CPPEVENT_EXAMPLES "Build example files" OFF)

if (CPPEVENT_TEST)
    add_subdirectory(libs/doctest)
endif()

add_subdirectory(cppevent_base)
add_subdirectory(cppevent_net)
add_subdirectory(cppevent_fcgi)

if (CPPEVENT_EXAMPLES)
    add_subdirectory(examples/timer)
    add_subdirectory(examples/echo)
    add_subdirectory(examples/telnet)
    add_subdirectory(examples/hello)
endif()
