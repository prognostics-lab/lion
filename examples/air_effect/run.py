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
    (A, B, C, _), params = lti_from_data(
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
    )
    LOGGER.info("\n")
    LOGGER.info(f"Final parameters: {params}")
    LOGGER.info(f"A = \n{A}")
    LOGGER.info(f"B = \n{B}")
    LOGGER.info(f"C = \n{C}")
    print(params)


def main():
    exp1_air = pd.read_csv(os.path.join("examples", "air_effect", "exp1_air_sim.csv")
    exp2_air = pd.read_csv(os.path.join("examples", "air_effect", "exp2_air_sim.csv")
    exp1_noair = pd.read_csv(os.path.join("examples", "air_effect", "exp1_noair_sim.csv")
    exp2_noair = pd.read_csv(os.path.join("examples", "air_effect", "exp2_noair_sim.csv")
    with open(os.path.join("examples", "air_effect", "params.json"), "r") as file:
        air_params = json.load(file)
    with open(os.path.join("examples", "air_effect", "noair_params.json"), "r") as file:
        noair_params = json.load(file)

