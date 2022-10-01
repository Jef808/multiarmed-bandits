include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB examples "${CMAKE_CURRENT_SOURCE_DIR}/ex_*.cpp")

add_subdirectory(reqrep)


foreach(ex ${examples})
  string(REPLACE ".cpp" "" ex_name ${ex})
  add_executable(${ex_name} ${ex})
endforeach()

add_executable(client_cmdline ${CMAKE_CURRENT_SOURCE_DIR}/client_cmdline.cpp)

#add_executable(${PROJECT_NAME}__cmdline ${CMAKE_CURRENT_SOURCE_DIR}/client_cmdline.cpp)
