import sys
import pathlib
import os
import importlib
import argparse

# Change the import path
src_path = pathlib.Path.joinpath(pathlib.Path(__file__).parent.parent, "src")
os.environ["PYTHONPATH"] = str(src_path)
sys.path.append(str(src_path))

from thermal_model.logger import setup_logger, LOGGER
from thermal_model.console import ShellColors


# Set up argument parsing
DESC_STR = """run the examples"""

parser = argparse.ArgumentParser(prog="examples", description=DESC_STR)
parser.add_argument(
    "args",
    nargs="+",
    help="program and its arguments",
)
parser.add_argument(
    "-d",
    "--debug",
    action="store_true",
    help="run in debug mode",
)
parser.add_argument(
    "--log",
    action="store",
    default=os.path.join(src_path.parent, "logs"),
    help="generate log files for the current run in the given directory",
)
parser.add_argument(
    "-q",
    "--quiet",
    action="store_true",
    help="disable logging",
)
parser.add_argument(
    "-v",
    "--verbose",
    action="store_true",
    help="run in verbose mode",
)

cmd_args = parser.parse_args()
os.makedirs(cmd_args.log, exist_ok=True)
setup_logger(cmd_args.quiet, cmd_args.debug, cmd_args.verbose, cmd_args.log)

# Get the attributes
name = cmd_args.args[0]
ex_args = []
ex_kwargs = {}
LOGGER.info("Parsing args and kwargs")
for arg in map(lambda x: x.split("="), cmd_args.args[1:]):
    if len(arg) == 2:
        key, val = arg
        ex_kwargs[key] = val
    else:
        if arg[0][0] == "+":
            ex_kwargs[arg[0][1:]] = True
        else:
            ex_args.extend(arg)

# Run the example
print(
    f"\nExecuting example '{ShellColors.GREEN}{name}{ShellColors.RESET}' \
with args {ex_args} and kwargs {ex_kwargs}\n"
)
LOGGER.info("Importing example %s", name)
mod = importlib.import_module(f"examples.{name}")
LOGGER.info("Running main function")
mod.main(*ex_args, **ex_kwargs)
