#!/usr/bin/env bash

cmake --build build --target MAB_epsilon_greedy
rm ./logs/multiarmed_bandits/*.json
./build/multiarmed_bandits/MAB_epsilon_greedy -a 10 -n 2000 -s 1000 -e 0.01
./build/multiarmed_bandits/MAB_epsilon_greedy -a 10 -n 2000 -s 1000 -e 0.1
./build/multiarmed_bandits/MAB_ucb -a 10 -n 2000 -s 1000 -c 2.0
./multiarmed_bandits/plot_ucb.py \
  ./logs/multiarmed_bandits/ucb_10_2000_1000_200.json \
  ./logs/multiarmed_bandits/epsilon_greedy_10_2000_1000_1.json \
  ./logs/multiarmed_bandits/epsilon_greedy_10_2000_1000_10.json
