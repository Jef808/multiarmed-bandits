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
    log_dir = Path(sys.argv[1])
    for f in log_dir.glob("*.json"):
        with open(f, 'r') as fin:
            data = json.load(fin)
            x = np.arange(0, data["n_steps"], 1)
            ax1.plot(x, data["rewards"], label=str(data["epsilon"]))
            ax2.plot(x, data["losses"], label=str(data["epsilon"]))
    ax1.legend(title="epsilon")
    ax2.legend(title="epsilon")
    plt.show()

if __name__ == '__main__':
    _main()
