project(backend LANGUAGES CXX VERSION 1.0)

message( "Configuring subdirectory ${CMAKE_CURRENT_SOURCE_DIR}\n" )

set(ZMQ_ROOT "/usr/")
#include (ext/azmq/config/FindZeroMQ.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set_property(DIRECTORY PROPERTY COMPILE_OPTIONS "-std=c++2a")
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED 17)

#add_library(server server.hpp server.cpp process_request.hpp process_request.cpp data_models.hpp data_models_helpers.hpp data.hpp)
#target_link_libraries(server PRIVATE ${ZeroMQ_LIBRARIES})

#message( ${rapidjson_DIR} )
#find_package(rapidjson REQUIRED)

add_executable(nl_handler nl_handler.h data_models.hpp nl_handler.cpp ../environments/multiarmed_bandits/multiarmed_bandits.cpp)
#target_link_libraries(nl_serde ${nlohmann_LIBRARIES})

# add_executable(rapidjson_parse rapidjson_handler.h rapidjson_handler.cpp)
# add_executable(rapidjson_serialize rapidjson_serialize.h rapidjson_serialize.cpp)

#add_subdirectory(__tests__)
