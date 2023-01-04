cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads)
set(Boost_DEBUG ON)
find_package(Boost REQUIRED COMPONENTS system)

message( STATUS "Boost: {Boost_FOUND}" )
message( STATUS "BOOST ROOT: ${BOOST_ROOT}" )
message( STATUS "BOOSTROOT: ${BOOSTROOT}" )
message( STATUS "Boost LIBS: ${Boost_LIBRARIES}" )
message( STATUS "Boost SYSTEM FOUND: ${Boost_SYSTEM_FOUND}" )
message( STATUS "Boost INCLUDE DIRS: ${Boost_INCLUDE_DIRS}" )


add_executable(server main.cpp request_handler.cpp)
target_link_libraries(server PRIVATE MAB boost_system Threads::Threads)
target_include_directories(server PRIVATE ${Boost_INCLUDE_DIRS})

# add_library(request_handle STATIC
#   request_handler.cpp
#   ${CMAKE_SOURCE_DIR}/environments/multiarmed_bandits/multiarmed_bandits.cpp)



# add_executable(serverbackend server.cpp)
# target_link_libraries(serverbackend PRIVATE request_handle boost_system Threads::Threads)
# target_include_directories(serverbackend PRIVATE ${Boost_INCLUDE_DIRS})

# add_executable(serversync serversync.cpp)
# target_link_libraries(serversync PRIVATE request_handle boost_system Threads::Threads)
# target_include_directories(serversync PRIVATE ${Boost_INCLUDE_DIRS})

# add_executable(client-example client_example.cpp)
# target_link_libraries(client-example PRIVATE boost_system Threads::Threads)
# target_include_directories(client-example PRIVATE ${Boost_INCLUDE_DIRS})
