set(multiarmed_bandits_DIR multiarmed_bandits)
set(tictactoe_DIR tictactoe)

add_library(Environments INTERFACE)

add_subdirectory(tictactoe)
add_subdirectory(multiarmed_bandits)


target_link_libraries(Environments INTERFACE Policies)
