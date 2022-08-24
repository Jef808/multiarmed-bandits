#!/usr/bin/env python3

import json
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
import sys

def _main():
    fig, (ax1, ax2, ax3) = plt.subplots(3, 1)
    ax1.sharex(ax2)
    fig.subplots_adjust(hspace=0.5)
    ax1.set_ylabel("Reward")
    ax2.set_ylabel("Loss")
    ax3.set_xlabel("Actions")
    ax3.set_ylabel("Expected Value")
    log_dir = Path(sys.argv[1])
    ax3_init = False
    for f in log_dir.glob("*.json"):
        with open(f, 'r') as fin:
            data = json.load(fin)
            x = np.arange(0, data["n_steps"], 1)
            ax1.plot(x, data["rewards"], label=str(data["epsilon"]))
            ax2.plot(x, data["losses"], label=str(data["epsilon"]))
            if not ax3_init:
                x3 = np.arange(0, data["n_actions"], 1)
                ax3.bar(x3, data["true_values"])
                ax3_init = True
    ax1.legend(title="epsilon")
    ax2.legend(title="epsilon")
    if (len(sys.argv) > 2):
        fout = sys.argv[2]
        plt.savefig(fout)
        print(f"Figure saved at {fout}")

if __name__ == '__main__':
    _main()
