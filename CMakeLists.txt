cmake_minimum_required(VERSION 3.5)

project(cppevent
    VERSION 1.0
    DESCRIPTION "cppevent library"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

option(CPPEVENT_TEST "Build test files" OFF)
option(CPPEVENT_EXAMPLES "Build example files" OFF)

add_subdirectory(src)

add_library(cppevent
  $<TARGET_OBJECTS:cppevent_base>
  $<TARGET_OBJECTS:cppevent_net>
  $<TARGET_OBJECTS:cppevent_fcgi>
)

target_include_directories(cppevent PUBLIC src)

if (CPPEVENT_EXAMPLES)
    add_subdirectory(examples)
endif()

if (CPPEVENT_TEST)
    add_subdirectory(libs/doctest)
    add_subdirectory(test)
endif()
