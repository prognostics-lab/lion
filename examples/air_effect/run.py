import numpy as np
from scipy import optimize
import pandas as pd

from thermal_model.estimation import error, lti_from_data
from thermal_model.logger import LOGGER


def get_rout(y: np.ndarray, u: np.ndarray) -> float
    return (y[-1, 0] - u[-1, 0]) / u[-1, 1]


def perform_experiment(exp1: pd.DataFrame, exp2: pd.DataFrame, x0, name=None) -> None:
    if name is None:
        name = "both"
    # Read timeseries from experiment 1
    y_exp1 = np.array([exp1["sf_temp"].to_numpy(), exp1["air_temp"].to_numpy()]).T
    u_exp1 = np.array([exp1["amb_temp"].to_numpy(), exp1["q_gen"].to_numpy()]).T
    t_exp1 = exp1["time"].to_numpy()
    rout = get_rout(y_exp1, u_exp1)

    # Read timeseries from experiment 2
    y = np.array([exp2["sf_temp"].to_numpy(), exp2["air_temp"].to_numpy()]).T
    u = np.array([exp2["amb_temp"].to_numpy(), exp2["q_gen"].to_numpy()]).T
    t = exp2["time"].to_numpy()

    # Estimate thermal parameters from the data
    (A, B, C, D), params = lti_from_data(
        y,
        u,
        t,
        x0,
        optimizer_kwargs={
            "fn": optimize.minimize,
            "method": "L-BFGS-B",
            "jac": "3-point",
            "tol": 1e-3,
            "options": {
                "disp": True,
                "maxiter": 1e2,
            },
            "err": error.l2,
        },
        system_kwargs={
            "outputs": name,
        },
    )
    LOGGER.info(f"Final parameters: {params}")
    LOGGER.debug(f"A = \n{A}")
    LOGGER.debug(f"B = \n{B}")
    LOGGER.debug(f"C = \n{C}")
    return A, B, C, D, params


def main():
    LOGGER.info("Reading data from air experiment")
    exp1_air = pd.read_csv(os.path.join("examples", "air_effect", "exp1_air_sim.csv")
    exp2_air = pd.read_csv(os.path.join("examples", "air_effect", "exp2_air_sim.csv")
    LOGGER.info("Reading data from noair experiment")
    exp1_noair = pd.read_csv(os.path.join("examples", "air_effect", "exp1_noair_sim.csv")
    exp2_noair = pd.read_csv(os.path.join("examples", "air_effect", "exp2_noair_sim.csv")
    LOGGER.info("Reading real parameters")
    with open(os.path.join("examples", "air_effect", "params.json"), "r") as file:
        air_params = json.load(file)
    with open(os.path.join("examples", "air_effect", "noair_params.json"), "r") as file:
        noair_params = json.load(file)

    LOGGER.debug("Creating initial conditions")
    x0_air = np.array([air_params["in_temp"], air_params["sf_temp"]])
    x0_noair = np.array([noair_params["in_temp"], noair_params["sf_temp"]])

    LOGGER.debug("Estimating parameters from air")
    A_air, B_air, C_air, D_air, params_air = perform_experiment(exp1_air, exp2_air, x0_air)

    LOGGER.debug("Estimating parameters from noair")
    A_noair, B_noair, C_noair, D_noair, params_noair = perform_experiment(exp1_air,exp2_air, x0_air, name="noair")

