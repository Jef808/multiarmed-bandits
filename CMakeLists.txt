cmake_minimum_required(VERSION 3.24)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)

find_package(fmt REQUIRED)

add_subdirectory(multiarmed_bandits)
add_subdirectory(qlearning)
add_subdirectory(samegame)
