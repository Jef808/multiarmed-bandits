#!/usr/bin/env bash

cmake --build build --target MAB_epsilon_greedy
rm ./logs/multiarmed_bandits/*.json
rm ./logs/multiarmed_bandits/*.png
images=()
N=$1
X=$((${N}*640/1920))
Y=$((${N}*480/1080))
for i in 1 2 3 4 5 6
do
    ./build/multiarmed_bandits/MAB_epsilon_greedy -e 0.01 -a 10 -n 500 -s 1000;
    ./build/multiarmed_bandits/MAB_epsilon_greedy -e 0.05 -a 10 -n 500 -s 1000;
    ./build/multiarmed_bandits/MAB_epsilon_greedy -e 0.1 -a 10 -n 500 -s 1000;
    ./build/multiarmed_bandits/MAB_epsilon_greedy -e 0.2 -a 10 -n 500 -s 1000;
    ./multiarmed_bandits/plot.py ./logs/multiarmed_bandits/ ./logs/multiarmed_bandits/MAB_EG_$i.png;
    images+=("./logs/multiarmed_bandits/MAB_EG_${i}.png")
done

#montage -geometry 600x480 MAB_EG_1.png MAB_EG_2.png MAB_EG_3.png MAB_EG_4.png MAB_EG_5.png MAB_EG_6.png MAB_EG_montage.png
montage -geometry 600x480 ${images[*]} ./logs/multiarmed_bandits/MAB_EG_montage.png

display ./logs/multiarmed_bandits/MAB_EG_montage.png
