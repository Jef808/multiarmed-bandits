set(env_DIR ${CMAKE_SOURCE_DIR}/environments)

include_directories(${env_DIR})
add_executable(qlearn_tictactoe
  ${env_DIR}/tictactoe.h
  ${env_DIR}/tictactoe.cpp
  qlearn_tictactoe.cpp)
