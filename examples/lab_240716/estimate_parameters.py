import os
import sys
import pathlib
import json

import numpy as np
from scipy import optimize
from matlab import engine

src_path = pathlib.Path.joinpath(pathlib.Path(os.getcwd()), "src")
datalib_path = pathlib.Path.joinpath(pathlib.Path(os.getcwd()), "data")
sys.path.append(str(src_path))
sys.path.append(str(datalib_path))
# pylint: disable=import-error
from thermal_model.estimation import lti_from_data, TargetParams, error
from thermal_model.logger import LOGGER
from thermal_model.paths import ML_PROJECTFILE

from lib_240716_temp_profile_C4B1 import get_data, Data

# pylint: enable=import-error


LAB_SLX_FILENAME = "lab_240716"


def perform_experiment(
    exp: Data,
    eng: engine.MatlabEngine,
    mdl,
    simin,
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
            "engine": eng,
            "mdl": mdl,
            "simin": simin,
            "initial_soc": 0,
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
    time_delta = data.t[-1] - data.t[-2]
    end_time = data.t[-1]
    LOGGER.debug(f"{time_delta=}, {end_time=}")

    LOGGER.info("Initializing MATLAB engine")
    eng = engine.start_matlab()
    LOGGER.debug("Loading project file")
    eng.matlab.project.loadProject(ML_PROJECTFILE)
    LOGGER.debug("Loading Simulink model")
    mdl = LAB_SLX_FILENAME
    simin = eng.py_load_model(
        mdl, time_delta, end_time, data.t, data.u[:, 1], data.u[:, 0]
    )

    LOGGER.info("Estimating parameters for experiment without air")
    *_, params = perform_experiment(
        data,
        eng,
        mdl,
        simin,
    )
    with open(
        os.path.join("examples", "lab_240716", "estimated_parameters.json"), "w"
    ) as f:
        f.write(json.dumps(params._asdict()))
