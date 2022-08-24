#!/usr/bin/env python3

import json
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
import sys

def _main():
    fig, (ax1, ax2) = plt.subplots(2, 1, sharex=True)
    fig.subplots_adjust(hspace=0.5)
    ax1.set_ylabel("Reward")
    ax2.set_ylabel("Loss")
    log_ucb = Path(sys.argv[1])
    log_epsilon_greedy = Path(sys.argv[2])
    log_epsilon_greedy2 = Path(sys.argv[3])
    with open(log_ucb, 'r') as fin:
        data = json.load(fin)
        x = np.arange(0, data["n_steps"], 1)
        ax1.plot(x, data["rewards"], label=f"UCB, C={data['exploration_constant']}")
        ax2.plot(x, data["losses"], label=f"UCB, C={data['exploration_constant']}")
    with open(log_epsilon_greedy, 'r') as fin:
        data = json.load(fin)
        x = np.arange(0, data["n_steps"], 1)
        ax1.plot(x, data["rewards"], label=f"Eps_Greedy, C={data['epsilon']}")
        ax2.plot(x, data["losses"], label=f"Eps_Greedy, C={data['epsilon']}")
    with open(log_epsilon_greedy2, 'r') as fin:
        data = json.load(fin)
        x = np.arange(0, data["n_steps"], 1)
        ax1.plot(x, data["rewards"], label=f"Eps_Greedy, eps={data['epsilon']}")
        ax2.plot(x, data["losses"], label=f"Eps_Greedy, eps={data['epsilon']}")
    ax1.legend(title="Policy")
    ax2.legend(title="Policy")
    plt.show()

if __name__ == '__main__':
    _main()
