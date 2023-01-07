cmake_minimum_required(VERSION 3.25)
project(mabviz LANGUAGES CXX VERSION 1.0)

set(BOOST_ROOT ${CMAKE_SOURCE_DIR}/ext/beast/include/boost)
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(this_DIR ${CMAKE_CURRENT_SOURCE_DIR})

option(mabviz_BUILD_TESTS "build tests")
option(mabviz_BUILD_ENVIRONMENTS "build environments")
option(mabviz_BUILD_BACKEND "build websocket backend")

set(mabviz_BUILD_TESTS ON)
set(mabviz_BUILD_BACKEND ON)
set(mabviz_BUILD_ENVIRONMENTS ON)

message(INFO "BUILD_TESTS: ${mabviz_BUILD_TESTS}")
message(INFO "BUILD_ENVIRONMENTS: ${mabviz_BUILD_ENVIRONMENTS}")
message(INFO "BUILD_BACKEND: ${mabviz_BUILD_BACKEND}")

include_directories(${CMAKE_SOURCE_DIR})

if (${mabviz_BUILD_TESTS})
    message(STATUS "Configuring tests...")
    include(Catch2)
    find_package(Catch2 REQUIRED)
    enable_testing()
    message(STATUS "Done configuring tests")
endif()

if (${mabviz_BUILD_BACKEND})
  message(STATUS "Configuring backend server...")
    include(boostbeast)
    add_subdirectory(serverbackend)
  message(STATUS "Done configuring backend server...")
endif()

if (${mabviz_BUILD_ENVIRONMENTS})
  message(STATUS "Configuring environments directory...")
  add_subdirectory(environments)
  add_subdirectory(policies)
  message(STATUS "Done configuring environments directory...")
endif()
