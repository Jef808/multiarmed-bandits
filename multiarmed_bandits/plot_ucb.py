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
    log_ucb2 = Path(sys.argv[2])
    log_ucb3 = Path(sys.argv[3])
    log_epsilon_greedy2 = Path(sys.argv[4])
    x_init = False
    with open(log_ucb, 'r') as fin:
        data = json.load(fin)
        if not x_init:
            x = np.arange(0, data["n_steps"], 1)
            x_init = True
        ax1.plot(x, data["rewards"], label=f"UCB, C={data['exploration_constant']}")
        ax2.plot(x, data["losses"], label=f"UCB, C={data['exploration_constant']}")
    with open(log_ucb2, 'r') as fin:
        data = json.load(fin)
        ax1.plot(x, data["rewards"], label=f"UCB, C={data['exploration_constant']}")
        ax2.plot(x, data["losses"], label=f"UCB, C={data['exploration_constant']}")
    with open(log_ucb3, 'r') as fin:
        data = json.load(fin)
        ax1.plot(x, data["rewards"], label=f"UCB, C={data['exploration_constant']}")
        ax2.plot(x, data["losses"], label=f"UCB, C={data['exploration_constant']}")
    with open(log_epsilon_greedy2, 'r') as fin:
        data = json.load(fin)
        ax1.plot(x, data["rewards"], label=f"Eps_Greedy, eps={data['epsilon']}")
        ax2.plot(x, data["losses"], label=f"Eps_Greedy, eps={data['epsilon']}")
    ax1.legend(title="Policy")
    ax2.legend(title="Policy")
    if len(sys.argv) > 5:
        fout = sys.argv[5]
        plt.savefig(fout)
        print(f"Figure saved as {fout}")

if __name__ == '__main__':
    _main()
