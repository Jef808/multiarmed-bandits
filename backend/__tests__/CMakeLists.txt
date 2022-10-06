project(backend_tests)

message( "Configuring subdirectory ${CMAKE_CURRENT_SOURCE_DIR}\n" )

if (NOT TARGET Catch2::Catch2)
  message("target Catch2::Catch2 not found")
endif()

message ("catch2 libs: ${Catch2_LIBRARY}" )
message ("cmake module path: ${CMAKE_MODULE_PATH}")

add_executable(tests_mockclient tests_mockclient.cpp)
target_link_libraries(tests_mockclient server ${ZeroMQ_LIBRARIES} Catch2::Catch2 Catch2::Catch2WithMain)
set_property(TARGET tests_mockclient PROPERTY CMAKE_CXX_STANDARD 17)

include(CTest)
include(Catch)
catch_discover_tests(tests_mockclient)
message( "Added test file tests_mockclient" )
