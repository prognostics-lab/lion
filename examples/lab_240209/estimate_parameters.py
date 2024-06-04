import os
import sys
import pathlib
import json

import numpy as np
from scipy import optimize

src_path = pathlib.Path.joinpath(pathlib.Path(os.getcwd()), "src")
datalib_path = pathlib.Path.joinpath(pathlib.Path(os.getcwd()), "data")
sys.path.append(str(src_path))
sys.path.append(str(datalib_path))
# pylint: disable=import-error
from thermal_model.estimation import lti_from_data, TargetParams, error
from thermal_model.logger import LOGGER

from lib_240209_temptest_C6B2 import get_data, Data

# pylint: enable=import-error


def perform_experiment(
    exp: Data,
    name=None,
    real_params=None,
    cair=0,
    rair=0,
) -> [np.ndarray, np.ndarray, np.ndarray, np.ndarray, TargetParams]:
    if name is None:
        name = "both"

    if name == "noair":
        y = exp.y[:, 0]
        x0 = exp.x0[0]
    else:
        y = exp.y
        x0 = exp.x0
    u = exp.u
    t = exp.t

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
    LOGGER.info("Getting data from experiment")
    train_data, _ = get_data(3, 2)

    LOGGER.info("Estimating parameters for experiment with air")
    *_, params_air = perform_experiment(
        train_data,
        name="both",
    )
    with open(os.path.join("examples", "lab_240209", "params_est_air.json"), "r") as f:
        f.write(json.dumps(params_air._asdict()))

    LOGGER.info("Estimating parameters for experiment without air")
    *_, params_noair = perform_experiment(
        train_data,
        name="noair",
    )
    with open(os.path.join("examples", "lab_240209", "params_est_noair.json"), "r") as f:
        f.write(json.dumps(params_noair._asdict()))
