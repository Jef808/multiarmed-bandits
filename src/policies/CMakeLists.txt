set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

add_library(Policies INTERFACE)
target_include_directories(Policies INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
