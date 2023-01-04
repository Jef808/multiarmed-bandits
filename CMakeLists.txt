cmake_minimum_required(VERSION 3.25)
project(mabviz LANGUAGES CXX VERSION 1.0)

set(BOOST_ROOT ${CMAKE_SOURCE_DIR}/ext/beast/include/boost)
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(this_DIR ${CMAKE_CURRENT_SOURCE_DIR})

option(mabviz_BUILD_TESTS ON)
option(mabviz_BUILD_ENVIRONMENTS ON)
option(mabviz_BUILD_BACKEND ON)

set(mabviz_BUILD_BACKEND ON)
set(mabviz_BUILD_ENVIRONMENTS ON)

include_directories(${CMAKE_SOURCE_DIR})

if (${mabviz_BUILD_TESTS})
    message(STATUS "Building tests...")
    include(Catch2)
    find_package(Catch2 REQUIRED)
    enable_testing()
endif()

if (${mabviz_BUILD_BACKEND})
    include(boostbeast)
    message(STATUS "Building server...")
    add_subdirectory(serverbackend)
endif()

if (${mabviz_BUILD_ENVIRONMENTS})
  add_subdirectory(environments)
  add_subdirectory(policies)
endif()
