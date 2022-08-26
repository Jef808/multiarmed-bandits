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
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]} <LOG_DIR> <greedy_bs.json> <OUTPUT.png>")
        sys.exit(1)
    log_dir = Path(sys.argv[1])
    greedy_baseline = Path(sys.argv[2])
    x_init = False
    for f in log_dir.glob("ucb*.json"):
        with open(f, 'r') as fin:
            data = json.load(fin)
            if not x_init:
                x = np.arange(0, data["n_steps"], 1)
                x_init = True
            ax1.plot(x, data["rewards"], label=f"UCB, C={data['exploration_constant']}")
            ax2.plot(x, data["losses"], label=f"UCB, C={data['exploration_constant']}")
    with open(greedy_baseline, 'r') as fin:
        data = json.load(fin)
        ax1.plot(x, data["rewards"], label=f"Epsilon_Greedy, eps={data['epsilon']}")
        ax2.plot(x, data["losses"], label=f"Epsilon_Greedy, eps={data['epsilon']}")
    ax1.legend(title="Policy")
    ax2.legend(title="Policy")
    fout = sys.argv[3]
    plt.savefig(fout)
    print(f"Figure saved as {fout}")

if __name__ == '__main__':
    _main()
