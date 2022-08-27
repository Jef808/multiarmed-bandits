project(multiarmed_bandits)

# Expose DEFAULT_CONFIG_FILEPATH string
set_property(DIRECTORY
  APPEND PROPERTY
  COMPILE_DEFINITIONS "DEFAULT_CONFIG_FILEPATH=\"${PROJECT_SOURCE_DIR}/config.cfg\"")

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

add_library(utils
  po_helper.h
  po_helper.cpp
  jsonlog.h
  jsonlog.cpp)
target_link_libraries(utils PUBLIC
  ${Boost_LIBRARIES})

add_executable(MAB_ucb_po
  test_ucb_po.cpp
  )
target_link_libraries(MAB_ucb_po PRIVATE
  MAB
  MAB_agents
  fmt::fmt
  utils)




# target_include_directories(MAB_epsilon_greedy PRIVATE
#   ${tensorboard_logger_INCLUDE_DIRS})
