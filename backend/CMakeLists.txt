project(backend LANGUAGES CXX VERSION 1.0)

message( "Configuring subdirectory ${CMAKE_CURRENT_SOURCE_DIR}\n" )

set(ZMQ_ROOT "/usr/")
include (ext/azmq/config/FindZeroMQ.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

add_library(server server.hpp server.cpp process_request.hpp process_request.cpp execute_request.hpp execute_request.cpp data.hpp)
target_link_libraries(server PRIVATE ${ZeroMQ_LIBRARIES})

add_subdirectory(__tests__)
