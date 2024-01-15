cmake_minimum_required(VERSION 3.5)

project(cppevent
    VERSION 1.0
    DESCRIPTION "cppevent library"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")

option(CPPEVENT_DEVEL "Developing cppevent" OFF)

add_subdirectory(src)

add_library(cppevent
  $<TARGET_OBJECTS:cppevent_base>
  $<TARGET_OBJECTS:cppevent_net>
  $<TARGET_OBJECTS:cppevent_fcgi>
)

target_include_directories(cppevent INTERFACE src)

include(CTest)

add_subdirectory(test)

if (CPPEVENT_DEVEL)
    add_subdirectory(examples)
else()
    install(TARGETS cppevent)
    install(DIRECTORY src/
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            FILES_MATCHING PATTERN "*.hpp")
endif()
