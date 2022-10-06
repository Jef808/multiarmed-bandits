project(backend LANGUAGES CXX VERSION 1.0)

message( "Configuring subdirectory ${CMAKE_CURRENT_SOURCE_DIR}\n" )

set(ZMQ_ROOT "/usr/")
include (ext/azmq/config/FindZeroMQ.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(server server.hpp server.cpp data.hpp)
set_property(TARGET server
  PROPERTY CXX_STANDARD 17)
target_link_libraries(server PRIVATE ${ZeroMQ_LIBRARIES})

add_subdirectory(__tests__)
