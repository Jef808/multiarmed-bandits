#!/usr/bin/env python3

import subprocess
from pathlib import Path


ROOTDIR = Path("/home/jfa/projects/ai")
BUILD_TARGETS = {"ucb": {"target": "MAB_ucb",
                         "executable": ROOTDIR / Path("build/multiarmed_bandits/MAB_ucb")},
                 "epsilon_greedy": {"target": "MAB_epsilon_greedy",
                                    "executable": ROOTDIR / Path("build/multiarmed_bandits/MAB_epsilon_greedy")}
                 }

LOGDIR = ROOTDIR / Path("app/public/data")
UCB_FP = f"{LOGDIR}/ucb"
UCB_EG = f"{LOGDIR}/epsilon_greedy"

GeneralArgs = {
    "actions":  {"arg": "-a", "default": 10, "value": 10},
    "episodes": {"arg": "-n", "default": 500, "value": 500},
    "steps":    {"arg": "-s", "default": 500, "value": 500},
    "stepsize": {"arg": "-j", "default": 10, "value": 10}
}

TargetArgs = {
    "ucb": {"exploration_constant": {"arg": "-c",
                                     "default": 0.4,
                                     "value": 0.4
                                     },
            "output": {"arg": "-o",
                       "default": UCB_FP,
                       "value":   UCB_FP}
            },
    "epsilon_greedy": {"epsilon": {"arg": "-e",
                                   "default": 0.01,
                                   "value":   0.01},
                       "output": {"arg": "-o",
                                  "default": UCB_EG,
                                  "value":   UCB_EG}
                       }
}


def get_exec_args(args):
    return [f"{v['arg']} {v['value']}" for v in args.values()]


def _main():

    # Run build
    cmake_exec_arglists = [["cmake", "--build", "build", "--target",
                            BUILD_TARGETS[T]['target']]
                           for T in ('ucb', 'epsilon_greedy')]

    # Run build
    for cmake_exec_arglist in cmake_exec_arglists:
        subproc = subprocess.run(cmake_exec_arglist,
                                 stdout=subprocess.PIPE,
                                 stderr=subprocess.STDOUT,
                                 text=True)
        print("stdout: ", subproc.stdout, "\nstderr: ", subproc.stderr)

    executable_ucb = BUILD_TARGETS['ucb']['executable']
    #executable_eg = BUILD_TARGETS['epsilon_greedy']['executable']

    # Generate epsilon_greedy data
    args_ucb = GeneralArgs.copy()
    #args_eg = GeneralArgs.copy()

    args_ucb.update(TargetArgs['ucb'].copy())
    #args_eg.update(TargetArgs['epsilon_greedy'].copy())

    # exec_arglist_ucb = [executable_ucb]
    # exec_arglist_ucb.extend(get_exec_args(args_ucb))
    # exec_arglist_eg = [executable_eg]
    # exec_arglist_eg.extend(get_exec_args(args_eg))

    # Generate data
    for i in range(20):
        args_ucb['exploration_constant']['value'] = args_ucb['exploration_constant']['default'] - 0.3 + i/10.0
        args_ucb['output']['value'] = UCB_FP + f"_{i}.json"
        exec_arglist_ucb = get_exec_args(args_ucb)
        arglist = [executable_ucb]
        arglist.extend(exec_arglist_ucb)
        subproc = subprocess.run(arglist,
                                 stdout=subprocess.PIPE,
                                 stderr=subprocess.STDOUT,
                                 text=True)
        print("stdout: ", subproc.stdout, "\nstderr: ", subproc.stderr)

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


if __name__ == '__main__':
    _main()
