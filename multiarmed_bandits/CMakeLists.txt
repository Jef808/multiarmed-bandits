add_library(multiarmed_bandit
  multiarmed_bandits.h
  multiarmed_bandits.cpp)


add_executable(MAB_epsilon_greedy
  agent_greedy.h
  agent_greedy.cpp
  test_epsilon_greedy.cpp)
target_link_libraries(MAB_epsilon_greedy PRIVATE
  multiarmed_bandit)
