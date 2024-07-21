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

from lib_240716_temp_profile_C4B1 import get_data, Data

# pylint: enable=import-error


def perform_experiment(
    exp: Data,
    real_params=None,
    cair=0,
    rair=0,
) -> [np.ndarray, np.ndarray, np.ndarray, np.ndarray, TargetParams]:
    y = exp.y[:, 0]
    x0 = exp.x0[0]
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
            "err": error.l2_simulation,
        },
        system_kwargs={
            "outputs": "noair",
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
    data = get_data()

    LOGGER.info("Estimating parameters for experiment without air")
    *_, params_noair = perform_experiment(
        data,
    )
    with open(os.path.join("examples", "lab_240716", "params_est_noair.json"), "w") as f:
        f.write(json.dumps(params_noair._asdict()))
