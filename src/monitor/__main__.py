import argparse
import os
import pathlib

from monitor.app import start
from monitor.logger import setup_logger


src_path = pathlib.Path.joinpath(pathlib.Path(__file__).parent.parent, "src")
DESC_STR = """run the monitor"""

parser = argparse.ArgumentParser(prog="monitor", description=DESC_STR)
parser.add_argument(
    "out",
    help="output file",
)
parser.add_argument(
    "port",
    help="serial port",
    default="COM1",
)
parser.add_argument(
    "-b",
    "--baud",
    help="baud rate",
    default=9600,
)
parser.add_argument(
    "-t",
    "--timeout",
    help="message timeout",
    default=5,
)
parser.add_argument(
    "-d",
    "--debug",
    action="store_true",
    help="run in debug mode",
)
parser.add_argument(
    "-q",
    "--quiet",
    action="store_true",
    help="disable logging",
)


cmd_args = parser.parse_args()
setup_logger(quiet=cmd_args.quiet, debug=cmd_args.debug, verbose=True)

# Start the program
start(cmd_args.out, cmd_args.port, int(cmd_args.baud), float(cmd_args.timeout))
