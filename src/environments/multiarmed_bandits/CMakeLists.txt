add_library(MAB multiarmed_bandits.h multiarmed_bandits.cpp)

target_link_libraries(Environments INTERFACE MAB)
