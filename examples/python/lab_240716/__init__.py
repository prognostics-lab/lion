import pandas as pd

from lion import App, Config, Params, Stepper
from lion_utils.logger import LOGGER


def main(power_filename, ambtemp_filename):
    LOGGER.info(f"Loading profiles, power: {power_filename}, amb: {ambtemp_filename}")
    power = pd.read_csv(power_filename)
    power = power[power.columns[0]].to_numpy()
    ambtemp = pd.read_csv(ambtemp_filename)
    ambtemp = ambtemp[ambtemp.columns[0]].to_numpy()

    LOGGER.info("Setting up configuration")
    conf = Config()
    conf.sim_step_seconds = 1
    conf.sim_epsabs = 1e-1
    conf.sim_epsrel = 1e-1
    conf.sim_min_maxiter = 10000

    LOGGER.info("Setting up parameters")
    params = Params()

    LOGGER.info("Running application")
    app = App(conf, params)
    app.run(power, ambtemp)

    LOGGER.info("Done!")
