import subprocess as sp

# pylint: disable=import-error
from thermal_model.logger import LOGGER
# pylint: enable=import-error

from .estimate_parameters import main as _main


def main():
    LOGGER.info("Generating simulated data")
    sp.run(["matlab", "-nodisplay", "-batch", "matlab.project.loadProject('battery_temperature_modelling.prj');", "gen_data.m"])

    LOGGER.info("Running parameter estimation on simulated data")
    _main()

    LOGGER.info("Evaluating air estimation")
    sp.run(["matlab", "-nodisplay", "-batch", "matlab.project.loadProject('battery_temperature_modelling.prj');", "eval_air_estimation.m"])

    LOGGER.info("Evaluating no-air estimation")
    sp.run(["matlab", "-nodisplay", "-batch", "matlab.project.loadProject('battery_temperature_modelling.prj');", "eval_noair_estimation.m"])


