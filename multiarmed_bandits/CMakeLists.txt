add_library(MAB
  multiarmed_bandits.h
  multiarmed_bandits.cpp)

add_library(MAB_agents INTERFACE
  policies.h
  agent.h)

add_executable(MAB_epsilon_greedy
  test_epsilon_greedy.cpp)
target_link_libraries(MAB_epsilon_greedy PRIVATE
  MAB
  MAB_agents
  fmt::fmt)

add_executable(MAB_ucb
  cmdline_helper.h
  test_ucb.cpp
  # cmdline_helper.cpp
  )
target_link_libraries(MAB_ucb PRIVATE
  MAB
  MAB_agents
  fmt::fmt)


# target_include_directories(MAB_epsilon_greedy PRIVATE
#   ${tensorboard_logger_INCLUDE_DIRS})
