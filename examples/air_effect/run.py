import os
import json

import numpy as np
from scipy import optimize
import pandas as pd

# pylint: disable=import-error
from thermal_model.estimation import error, lti_from_data, TargetParams
from thermal_model.logger import LOGGER
from thermal_model.console import ShellColors

# pylint: enable=import-error

_ERROR_THRESHOLD = 0.1


def get_rout(y: np.ndarray, u: np.ndarray, name: str) -> float:
    if name == "noair":
        return (y[-1] - u[-1, 0]) / u[-1, 1]
    else:
        return (y[-1, 0] - u[-1, 0]) / u[-1, 1]


def perform_experiment(
    exp1: pd.DataFrame,
    exp2: pd.DataFrame,
    x0,
    name=None,
    real_params=None,
    cair=0,
    rair=0,
) -> None:
    if name is None:
        name = "both"

    if name == "noair":
        y_exp1 = exp1["sf_temp"].to_numpy()
        y = exp2["sf_temp"].to_numpy()
    else:
        y_exp1 = np.array([exp1["sf_temp"].to_numpy(), exp1["air_temp"].to_numpy()]).T
        y = np.array([exp2["sf_temp"].to_numpy(), exp2["air_temp"].to_numpy()]).T
    # Read timeseries from experiment 1
    u_exp1 = np.array([exp1["amb_temp"].to_numpy(), exp1["q_gen"].to_numpy()]).T
    t_exp1 = exp1["time"].to_numpy()
    rout = get_rout(y_exp1, u_exp1, name)

    # Read timeseries from experiment 2
    u = np.array([exp2["amb_temp"].to_numpy(), exp2["q_gen"].to_numpy()]).T
    t = exp2["time"].to_numpy()

    # Estimate thermal parameters from the data
    (A, B, C, D), params = lti_from_data(
        y,
        u,
        t,
        x0,
        initial_guess=TargetParams(
            cp=1,
            cair=cair,
            rin=1,
            rout=1,
            rair=rair,
        ),
        fixed_params={
            "cair": cair,
            # "rout": rout,
            "rair": rair,
        },
        optimizer_kwargs={
            "fn": optimize.minimize,
            "method": "L-BFGS-B",
            "jac": "3-point",
            "tol": 1e-3,
            "options": {
                "disp": True,
                "maxiter": 1e3,
            },
            "err": error.l2,
        },
        system_kwargs={
            "outputs": name,
        },
    )
    LOGGER.info(f"Final parameters: {params}")
    if real_params is not None:
        LOGGER.info(f"Real parameters: {real_params}")
    LOGGER.debug(f"A = \n{A}")
    LOGGER.debug(f"B = \n{B}")
    LOGGER.debug(f"C = \n{C}")
    LOGGER.debug(f"D = \n{D}")
    return A, B, C, D, params


def main():
    LOGGER.info("Reading data from air experiment")
    exp1_air = pd.read_csv(os.path.join("examples", "air_effect", "exp1_air_sim.csv"))
    exp2_air = pd.read_csv(os.path.join("examples", "air_effect", "exp2_air_sim.csv"))
    LOGGER.info("Reading data from noair experiment")
    exp1_noair = pd.read_csv(
        os.path.join("examples", "air_effect", "exp1_noair_sim.csv")
    )
    exp2_noair = pd.read_csv(
        os.path.join("examples", "air_effect", "exp2_noair_sim.csv")
    )
    LOGGER.info("Reading real parameters")
    with open(os.path.join("examples", "air_effect", "air_params.json"), "r") as file:
        air_params = json.load(file)
    with open(os.path.join("examples", "air_effect", "noair_params.json"), "r") as file:
        noair_params = json.load(file)

    LOGGER.debug("Creating initial conditions")
    x0_air = np.array([air_params["in_temp"], air_params["air_temp"]])
    x0_noair = np.array([noair_params["in_temp"]])
    cair_air = air_params["cair"]
    rair_air = air_params["rair"]
    cair_noair = noair_params["cair"]
    rair_noair = noair_params["rair"]

    ### Experiment with air ###
    LOGGER.debug("Estimating parameters for experiment with air")
    A_air, B_air, C_air, D_air, params_air = perform_experiment(
        exp1_air,
        exp2_air,
        x0_air,
        name="both",
        cair=cair_air,
        rair=rair_air,
        real_params=air_params,
    )
    LOGGER.debug("Validating parameters for experiment with air")
    _names = ["cp", "rin", "rout"]
    params_air_arr = np.array([params_air.cp, params_air.rin, params_air.rout])
    real_params_air_arr = np.array(
        [air_params["cp"], air_params["rin"], air_params["rout"]]
    )
    air_percentual_error = (
        np.abs(real_params_air_arr - params_air_arr) / real_params_air_arr
    )
    print("Percentual errors, with air")
    print("---------------------------")
    found_error = False
    for n, err in zip(_names, air_percentual_error):
        percent_err = 100 * err
        if err >= _ERROR_THRESHOLD:
            found_error = True
            print(
                f" + {n} -> {ShellColors.RED} {percent_err:7.2f}% >= {100 * _ERROR_THRESHOLD:3.0f}%{ShellColors.RESET}"
            )
        else:
            print(f" + {n} -> {percent_err:7.2f}%")
    if found_error:
        LOGGER.warning("Found an error in the estimated parameters")

    ### Experiment without air ###
    LOGGER.debug("Estimating parameters from noair")
    A_noair, B_noair, C_noair, D_noair, params_noair = perform_experiment(
        exp1_noair,
        exp2_noair,
        x0_noair,
        name="noair",
        cair=cair_noair,
        rair=rair_noair,
        real_params=noair_params,
    )
    _names = ["cp", "rin", "rout"]
    params_noair_arr = np.array([params_noair.cp, params_noair.rin, params_noair.rout])
    real_params_noair_arr = np.array(
        [noair_params["cp"], noair_params["rin"], noair_params["rout"]]
    )
    noair_percentual_error = (
        np.abs(real_params_noair_arr - params_noair_arr) / real_params_noair_arr
    )
    print()
    print("Percentual errors, without air")
    print("------------------------------")
    found_error = False
    for n, err in zip(_names, noair_percentual_error):
        percent_err = 100 * err
        if err >= _ERROR_THRESHOLD:
            found_error = True
            print(
                f" + {n:>5} -> {ShellColors.RED}{percent_err:9.2f}% >= {100 * _ERROR_THRESHOLD:3.0f}%{ShellColors.RESET}"
            )
        else:
            print(f" + {n:>5} -> {percent_err:7.2f}%")
    if found_error:
        LOGGER.warning("Found an error in the estimated parameters")
