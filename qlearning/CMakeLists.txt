set(env_DIR ${CMAKE_SOURCE_DIR}/environments)

add_executable(qlearn_tictactoe
  ${env_DIR}/tictactoe.h
  ${env_DIR}/tictactoe.cpp
  qlearn_tictactoe.cpp)

target_include_directories(qlearn_tictactoe PUBLIC
  ${CMAKE_SOURCE_DIR})
