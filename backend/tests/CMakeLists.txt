project(tests)

message("backend_DIR: " ${backend_DIR})

# include_directories(${backend_DIR} ${ZeroMQ_INCLUDE_DIRS})
# link_libraries(${ZeroMQ_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_subdirectory(jsonrpc)
# add_subdirectory(reqrep)
# add_subdirectory(boost_describe)
