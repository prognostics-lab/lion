import subprocess as sp

# pylint: disable=import-error
from thermal_model.logger import LOGGER

# pylint: enable=import-error

from .estimate_parameters import main as _main


_MLCALL = "matlab -nodisplay -batch \"matlab.project.loadProject('battery_temperature_modelling.prj'); {}\""


def _call_matlab_script(cmd):
    with sp.Popen(cmd, stdout=sp.PIPE, universal_newlines=True, shell=True) as p:
        for l in iter(p.stdout.readline, ""):
            LOGGER.debug("[MATLAB] %s", l[:-1] if l.endswith("\n") else l)


def main():
    LOGGER.info("Generating simulated data")
    _call_matlab_script([_MLCALL.format("gen_data")])

    LOGGER.info("Running parameter estimation on simulated data")
    _main()

    LOGGER.info("Evaluating air estimation")
    _call_matlab_script([_MLCALL.format("eval_air_estimation")])

    LOGGER.info("Evaluating no-air estimation")
