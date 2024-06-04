import subprocess as sp

# pylint: disable=import-error
from thermal_model.logger import LOGGER

# pylint: enable=import-error

from .estimate_parameters import main as _main


_MLCALL = "matlab -nodisplay -batch \"matlab.project.loadProject('battery_temperature_modelling.prj'); {}\""


def main():
    LOGGER.info("Generating simulated data")
    sp.run(
        [_MLCALL.format("gen_data")],
        shell=True,
        capture_output=True,
    )

    LOGGER.info("Running parameter estimation on simulated data")
    _main()

    LOGGER.info("Evaluating air estimation")
    sp.run(
        [_MLCALL.format("eval_air_estimation")],
        shell=True,
        capture_output=True,
    )

    LOGGER.info("Evaluating no-air estimation")
    sp.run(
        [_MLCALL.format("eval_noair_estimation")],
        shell=True,
        capture_output=True,
    )
