#!/usr/bin/env python3

import subprocess
from pathlib import Path


ROOTDIR = Path("/home/jfa/projects/ai")
BUILD_TARGETS = {
    "ucb": {
        "target": "MAB_ucb_po",
        "executable": ROOTDIR / Path("build/multiarmed_bandits/MAB_ucb"),
    },
    "epsilon_greedy": {
        "target": "MAB_epsilon_greedy",
        "executable": ROOTDIR / Path("build/multiarmed_bandits/MAB_epsilon_greedy"),
    },
}

LOGDIR = ROOTDIR / Path("app/public/data")
UCB_FP = f"{LOGDIR}/ucb"
UCB_EG = f"{LOGDIR}/epsilon_greedy"

GeneralArgs = {
    "actions": {"arg": "-a", "value": 10},
    "episodes": {"arg": "-n", "value": 2000},
    "steps": {"arg": "-s", "value": 1000},
    "stepsize": {"arg": "-j", "value": 1},
}

TargetArgs = {
    "ucb": {"exploration_constant": {"arg": "-c", "value": 0.1}},
    "epsilon_greedy": {
        "epsilon": {"arg": "-e", "value": 0.01},
        "output": {"arg": "-o", "value": UCB_EG},
    },
}


def get_exec_args(args):
    return [str(v) for arg in args.values() for v in arg.values()]


def _main():

    # Run build
    cmake_exec_arglists = [
        ["cmake", "--build", "build", "--target", BUILD_TARGETS[T]["target"]]
        for T in ("ucb", "epsilon_greedy")
    ]

    # Run build
    for cmake_exec_arglist in cmake_exec_arglists:
        subproc = subprocess.run(
            cmake_exec_arglist,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
        )
        print("\nstdout: ", subproc.stdout)
        print("\nstderr: ", subproc.stderr)

    executable_ucb = BUILD_TARGETS["ucb"]["executable"]
    # executable_eg = BUILD_TARGETS['epsilon_greedy']['executable']

    args_ucb = GeneralArgs.copy()
    # args_eg = GeneralArgs.copy()

    args_ucb.update(TargetArgs["ucb"].copy())
    # args_eg.update(TargetArgs['epsilon_greedy'].copy())

    # Generate data
    for i in range(10):
        exec_arglist_ucb = get_exec_args(args_ucb)
        exec_arglist_ucb.append(UCB_FP + f"_{i}.json")
        print("Calling subprocess with following arguments:", exec_arglist_ucb)
        arglist = [executable_ucb]
        arglist.extend(exec_arglist_ucb)
        subproc = subprocess.run(
            arglist, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True
        )
        print("\nstdout: ", subproc.stdout)
        print("\nstderr: ", subproc.stderr)
        args_ucb["exploration_constant"]["value"] += 0.1

    # for i in range(10):
    #     args_eg['epsilon']['value'] += 0.02 * (i + 1)
    #     args_eg['output']['value'] = args_eg['output']['default'] + f"_{i}.json"
    #     subproc = subprocess.run(exec_arglist_eg,
    #                              stdout=subprocess.PIPE,
    #                              stderr=subprocess.STDOUT,
    #                              text=True)

    # print("stdout: ", subproc.stdout, "\nstderr: ", subproc.stderr)
    # Run plot script
    # plot_script = ROOTDIR / "./multiarmed_bandits/plot_ucb.py"
    # output = LOGDIR + f"{TARGET['name']}_{output_id}.png"
    # exec_arglist = [] + outputs + [output]
    # subprocess.run([plot_script].extend(exec_arglist), stdout = subprocess.PIPE)

    # Display
    # subprocess.run(["display", output])


if __name__ == "__main__":
    _main()
