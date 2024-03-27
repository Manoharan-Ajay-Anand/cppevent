cmake_minimum_required(VERSION 3.5)

project(cppevent
    VERSION 1.0
    DESCRIPTION "cppevent library"
    LANGUAGES CXX)

option(CPPEVENT_DEVEL "Developing cppevent" OFF)

add_subdirectory(src)

add_library(cppevent
  $<TARGET_OBJECTS:cppevent_base>
  $<TARGET_OBJECTS:cppevent_net>
  $<TARGET_OBJECTS:cppevent_fcgi>
)

target_include_directories(cppevent INTERFACE src)

if (BUILD_TESTING)
    include(CTest) 
    add_subdirectory(test)
endif()

if (CPPEVENT_DEVEL)
    add_subdirectory(examples)
endif()
