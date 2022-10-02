cmake_minimum_required(VERSION 3.24)
project(mabviz)

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

find_package(Catch2 REQUIRED)
include(CTest)
include(Catch)


# Expose VIEWER_DATA_DIR string
set_property(DIRECTORY
  APPEND PROPERTY
  COMPILE_DEFINITIONS "IN_VIEWER_DATA_DIR=\"${PROJECT_SOURCE_DIR}/app/public/data/\"")

include_directories(${CMAKE_SOURCE_DIR})

# Subdirectories
add_subdirectory(multiarmed_bandits)
add_subdirectory(qlearning)
add_subdirectory(samegame)
add_subdirectory(backend)
