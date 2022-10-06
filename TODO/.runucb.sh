#!/usr/bin/env bash

cmake --build build --target MAB_epsilon_greedy

rm ./logs/multiarmed_bandits/ucb*
rm ./logs/multiarmed_bandits/MAB_UCB*

images=()

for i in 1 2 3
do
    ./build/multiarmed_bandits/MAB_epsilon_greedy -a 10 -n 500 -s 200 -e 0.01 -j 10
    ./build/multiarmed_bandits/MAB_ucb -a 10 -n 500 -s 200 -j 10 -c 0.40
    ./build/multiarmed_bandits/MAB_ucb -a 10 -n 500 -s 200 -j 20 -c 0.40
    ./build/multiarmed_bandits/MAB_ucb -a 10 -n 500 -s 200 -j 1 -c 0.40

    ./multiarmed_bandits/plot_ucb.py \
        ./logs/multiarmed_bandits/ucb_10_500_200_10_40.json \
        ./logs/multiarmed_bandits/ucb_10_500_200_20_40.json \
        ./logs/multiarmed_bandits/ucb_10_500_200_1_40.json \
        ./logs/multiarmed_bandits/epsilon_greedy_10_500_200_10_1.json \
        ./logs/multiarmed_bandits/MAB_UCB_$i.png

    image+=("./logs/multiarmed_bandits/MAB_UCB_${i}.png")
done

montage -geometry 600x480 ${images[*]} ./logs/multiarmed_bandits/MAB_UCB_montage.png

display ./logs/multiarmed_bandits/MAB_UCB_montage.png
