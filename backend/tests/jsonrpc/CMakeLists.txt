project(jsonrpc)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

#set(jsonrpc_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(tests tests.cpp json_recv.hpp json_call.hpp json_validate.hpp)
target_include_directories(tests PUBLIC ${backend_DIR} ${ZeroMQ_INCLUDE_DIRS})
target_link_libraries(tests PUBLIC ${ZeroMQ_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

message("   ZMQ INCLUDES: ${ZeroMQ_INCLUDE_DIRS}   ZMQ_LIBS: ${ZeroMQ_LIBRARIES}  THREAD_LIBS_INIT:  ${CMAKE_THREAD_LIBS_INIT}")

include(CTest)
include(Catch)
catch_discover_tests(tests)
