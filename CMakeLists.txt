cmake_minimum_required(VERSION 3.24)
project(mabviz LANGUAGES CXX VERSION 1.0)

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(this_DIR ${CMAKE_CURRENT_SOURCE_DIR})

option(mabviz_BUILD_TESTS ON)
option(mabviz_BUILD_ENVIRONMENTS OFF)

include(Catch2)
find_package(Catch2 REQUIRED)
enable_testing()

add_subdirectory(backend)

if (${mabviz_BUILD_ENVIRONMENTS})
  add_subdirectory(environments)
  else()
    message("Skipped subdirectory 'environments'\n")
endif()
