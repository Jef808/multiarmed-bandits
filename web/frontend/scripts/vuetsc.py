import re
import subprocess
import shlex
from rich.console import Console
from rich.syntax import Syntax

console = Console()


def print_error(error_obj):
    console.rule(f"[bold red]{error_obj['src_path']}")
    with open(error_obj['src_path'], "rt") as code_file:
        syntax = Syntax(code_file.read(),
                        "javascript",
                        line_numbers=True,
                        line_range=(int(error_obj['line_nb'])-5,
                                    int(error_obj['line_nb'])+5))
    console.print(syntax)
    console.print(f"[bold red]Line {error_obj['line_nb']}: [bold yellow]{error_obj['desc']}\n")


def handle_error(error):
    error_obj = {
        "src_path": "",
        "line_nb": 0,
        "column_nb": 0,
        "error_code": 0,
        "desc": ""
    }
    error_pattern = re.compile(
        r"""(?P<src_path>[^\.]+\.[^\.]+)  # the path of source file
        \((?P<line_nb>\d+),                # the line number
        (?P<column_nb>\d+)\):              # the column number
        \serror\s (?P<error_code>[^:]+):   # the error code
        \s(?P<desc>.+)$                    # the description of the error
        """, re.X)
    match = error_pattern.match(error)
    if match is not None:
        error_obj.update({
            "src_path": match['src_path'],
            "line_nb": match['line_nb'],
            "column_nb": match['column_nb'],
            "error_code": match['error_code'],
            "desc": match['desc']
        })
        print_error(error_obj)


if __name__ == '__main__':
    command = shlex.split("node node_modules/vue-tsc/bin/vue-tsc.js --noEmit")
    process = subprocess.Popen(command, stdout=subprocess.PIPE, text=True)
    outs, errs = process.communicate(timeout=15)

    console.print("\n\n")
    for line in outs.rstrip().splitlines():
        handle_error(line.strip())
