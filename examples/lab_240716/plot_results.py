import os
import sys
import pathlib
import json

import matplotlib.pyplot as plt
import numpy as np
from matlab import engine

src_path = pathlib.Path.joinpath(pathlib.Path(os.getcwd()), "src")
datalib_path = pathlib.Path.joinpath(pathlib.Path(os.getcwd()), "data")
sys.path.append(str(src_path))
sys.path.append(str(datalib_path))
# pylint: disable=import-error
from thermal_model.estimation import lti_from_data, TargetParams, error
from thermal_model.logger import LOGGER
from thermal_model.paths import ML_PROJECTFILE

from lib_240716_temp_profile_C4B1 import get_data, Data, INITIAL_SOC

# pylint: enable=import-error

from .constants import LAB_SLX_FILENAME


def main(savefig=False):
    LOGGER.info("Getting data from experiment")
    data = get_data()
    time_delta = data.t[-1] - data.t[-2]
    end_time = data.t[-1]

    time = data.t
    power = data.u[:, 1]
    amb_temp = data.u[:, 0]
    sf_temp = data.y[1:, 0]
    initial_conditions = {
        "initial_in_temp": data.x0,
        "initial_soc": INITIAL_SOC,
    }
    LOGGER.debug(f"{time_delta=}, {end_time=}")

    LOGGER.info("Reading estimated parameters")
    with open(os.path.join("examples", "lab_240716", "estimated_parameters.json"), "r") as f:
        params = json.load(f)

    LOGGER.info("Initializing MATLAB engine")
    eng = engine.start_matlab()
    LOGGER.debug("Loading project file")
    eng.matlab.project.loadProject(ML_PROJECTFILE)
    LOGGER.debug("Loading Simulink model")
    mdl = LAB_SLX_FILENAME
    simin = eng.py_load_model(mdl, time_delta, end_time, time, power, amb_temp)

    LOGGER.info("Calling simulation")
    simout = eng.py_evaluate_model(mdl, simin, params, initial_conditions)
    simout = np.array(simout)
    sim_time = simout[:, 0]
    sim_in_temp = simout[:, 1]
    sim_sf_temp = simout[:, 2]
    time = time[1:]  # why???

    LOGGER.info("Calculating error metrics")
    error = sf_temp - sim_sf_temp
    mse = np.mean(error ** 2)
    rmse = np.sqrt(mse)
    mae = np.mean(np.abs(error))
    print("Error metrics")
    print("=============")
    print(f" + RMSE -> {rmse}")
    print(f" + MAE  -> {mae}")

    LOGGER.info("Generating plots")
    fig, ax = plt.subplots(2, 1, sharex=True)

    ax[0].plot(sim_time / 3600, sim_sf_temp, label="Estimated")
    ax[0].plot(time / 3600, sf_temp, alpha=0.5, label="Measured")
    ax[0].legend()
    ax[0].set_xlabel("Time (h)")
    ax[0].set_ylabel("Temperature (°C)")
    ax[0].set_title("Surface temperature")

    ax[1].plot(time / 3600, (sf_temp - sim_sf_temp) ** 2, alpha=0.5, label="Square error")
    ax[1].legend()
    ax[1].set_xlabel("Time (h)")
    ax[1].set_ylabel("Square error (°C^2)")
    ax[1].set_title("Estimation error")
    ax[1].set_yscale("log")

