cmake_minimum_required(VERSION 3.21)

project(cppevent
    VERSION 1.0
    DESCRIPTION "cppevent library"
    LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL)
  include(CTest)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")
endif()

add_subdirectory(src)

add_library(cppevent
  $<TARGET_OBJECTS:cppevent_base>
  $<TARGET_OBJECTS:cppevent_net>
  $<TARGET_OBJECTS:cppevent_fcgi>
)

target_include_directories(cppevent INTERFACE src)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    install(TARGETS cppevent)
    install(DIRECTORY src/
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            FILES_MATCHING PATTERN "*.hpp")
endif()

if (BUILD_TESTING)
    add_subdirectory(test)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(examples)
endif()
