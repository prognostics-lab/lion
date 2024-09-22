import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

from lion import App, Config, Params, Stepper, Status, State
from lion_utils.logger import LOGGER


# TODO: See how to remove the global variable
STATE_SIZE = 17
_mat = None


def init_hook(app: App) -> Status:
    print("*** Init hook called ***")
    return Status.SUCCESS


def update_hook(app: App) -> Status:
    # Updates the data matrix
    global _mat
    _mat[app.state.step] = app.state.as_numpy()
    return Status.SUCCESS


def finished_hook(app: App) -> Status:
    print("*** Finished hook called ***")
    return Status.SUCCESS


def main(power_filename, ambtemp_filename, show=False, save=False):
    global _mat

    LOGGER.info(f"Loading profiles, power: {power_filename}, amb: {ambtemp_filename}")
    power = pd.read_csv(power_filename)
    power = power[power.columns[0]].to_numpy()
    ambtemp = pd.read_csv(ambtemp_filename)
    ambtemp = ambtemp[ambtemp.columns[0]].to_numpy()
    _mat = np.zeros((len(power) - 1, STATE_SIZE))  # why the -1???

    LOGGER.info("Setting up configuration")
    conf = Config()
    conf.sim_step_seconds = 1
    conf.sim_epsabs = 1e-1
    conf.sim_epsrel = 1e-1
    conf.sim_min_maxiter = 1000000

    LOGGER.info("Setting up parameters")
    params = Params()

    LOGGER.info("Running application")
    app = App(conf, params)
    app.init_hook = init_hook
    app.update_hook = update_hook
    app.finished_hook = finished_hook
    app.run(power, ambtemp)

    LOGGER.info("Processing data")
    print(_mat)

    df = pd.DataFrame(_mat, columns=State.get_keys())
    print(df)
    _plot_data(df, save)
    if show:
        plt.show()


def _plot_data(df, save=False):
    time = df["time"]

    fig, ax = plt.subplots(3, 1)
    fig.suptitle("Electrical behavior")
    ax[0].plot(time, df["voltage"])
    ax[1].plot(time, df["open_circuit_voltage"])
    ax[2].plot(time, df["current"])
    ax[0].set_title("Terminal voltage")
    ax[1].set_title("OCV")
    ax[2].set_title("Current")

    fig, ax = plt.subplots(3, 1)
    fig.suptitle("Charge and stuff")
    ax[0].plot(time, df["internal_resistance"])
    ax[1].plot(time, df["soc_use"])
    ax[2].plot(time, df["soc_nominal"])
    ax[0].set_title("Internal resistance")
    ax[1].set_title("Usable SoC")
    ax[2].set_title("Nominal SoC")

    fig, ax = plt.subplots(3, 1)
    fig.suptitle("Temperature")
    ax[0].plot(time, df["ambient_temperature"] - 273)
    ax[1].plot(time, df["surface_temperature"] - 273)
    ax[2].plot(time, df["internal_temperature"] - 273)
    ax[0].set_title("Ambient")
    ax[1].set_title("Surface")
    ax[2].set_title("Internal")
