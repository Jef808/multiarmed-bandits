#!/usr/bin/env bash

cmake --build build --target MAB_ucb
rm ./logs/multiarmed_bandits/ucb*.json
rm ./logs/multiarmed_bandits/MAB_UCB*.png
images=()

declare -i NActions=10 NEpisodes=500 NSteps=200 StepSize=10
declare -i C=20

for i in 1 2 3
do
    declare -i ExpCst11=ExpCst1 ExpCst12=ExpCst11 + Delta1  ExpCst13=ExpCst12 + Delta1

    ./build/multiarmed_bandits/MAB_epsilon_greedy -a $NActions -n $NEpisodes -s $NSteps -j $StepSize -e $Epsilon100.$Epsilon1
    ./build/multiarmed_bandits/MAB_ucb -a $NActions -n $NEpisodes -s $NSteps -j $StepSize -c $C
    ./build/multiarmed_bandits/MAB_ucb -a $NActions -n $NEpisodes -s $NSteps -j $StepSize -c $C
    ./build/multiarmed_bandits/MAB_ucb -a $NActions -n $NEpisodes -s $NSteps -j $StepSize -c $C
    ./multiarmed_bandits/plot_ucb.py ./logs/multiarmed_bandits/ ./logs/multiarmed_bandits/epsilon_greedy_500_200_10_1.json ./logs/multiarmedbandits/MAB_UCB_$i.png
    image+=("./logs/multiarmed_bandits/MAB_UCB_${i}.png")
done

montage -geometry 600x480 ${images[*]} ./logs/multiarmed_bandits/MAB_UCB_montage.png
display ./logs/multiarmed_bandits/MAB_UCB_montage.png
