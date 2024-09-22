import pandas as pd
from tqdm import tqdm

from lion import App, Config, Params, Stepper
from lion_utils.logger import LOGGER


def main(power_filename, ambtemp_filename):
    LOGGER.info(f"Loading profiles, power: {power_filename}, amb: {ambtemp_filename}")
    power = pd.read_csv(power_filename)
    power = power[power.columns[0]].to_numpy()
    ambtemp = pd.read_csv(ambtemp_filename)
    ambtemp = ambtemp[ambtemp.columns[0]].to_numpy()
    df = None

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

    for p, t in tqdm(zip(power, ambtemp), "Stepping", total=len(power)):
        app.step(p, t)
        _dict = {key: [val] for key, val in app.state.as_dict().items()}
        df = pd.concat([df, pd.DataFrame(_dict)], ignore_index=True)

    LOGGER.info("Done!")
    print(df)
