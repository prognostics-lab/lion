import json
import os
import pathlib
import sys

import numpy as np
# import optimparallel
from matlab import engine
from scipy import optimize

src_path = pathlib.Path.joinpath(pathlib.Path(os.getcwd()), "pysrc")
datalib_path = pathlib.Path.joinpath(pathlib.Path(os.getcwd()), "data")
sys.path.append(str(src_path))
sys.path.append(str(datalib_path))
from lib_240716_temp_profile_C4B1 import (Data, cell_capacity,
                                          cell_initial_soc,
                                          cell_internal_resistance,
                                          chamber_pv_std, get_data,
                                          temp_sensor_std)
# pylint: disable=import-error
from thermal_model.estimation import TargetParams, error, lti_from_data
from thermal_model.logger import LOGGER
from thermal_model.paths import MATLAB_PROJECTFILE

from .constants import LAB_SLX_FILENAME

# pylint: enable=import-error


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
            cp=100,
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
            # "fn": optimparallel.minimize_parallel,
            "jac": "3-point",
            "tol": 1e-3,
            "options": {
                "disp": True,
                "maxiter": 1e3,
            },
            "err": error.ErrorL2Simulation,
        },
        system_kwargs={
            "outputs": "noair",
            "engine": eng,
            "mdl": mdl,
            "simin": simin,
            "initial_soc": cell_initial_soc,
            "internal_resistance": cell_internal_resistance,
            "nominal_capacity": cell_capacity,
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
    data = get_data(cutoff=None)
    time_delta = data.t[-1] - data.t[-2]
    end_time = data.t[-1]

    time = data.t
    power = data.u[:, 1]
    amb_temp = data.u[:, 0]
    LOGGER.debug(f"{time_delta=}, {end_time=}")
    LOGGER.debug(f"Internal resistance = {cell_internal_resistance} Ohm")
    LOGGER.debug(f"Nominal capacity = {cell_capacity} C ({cell_capacity / 3600} Ah)")
    LOGGER.debug(f"Initial SOC = {cell_initial_soc} ({100 * cell_initial_soc} %)")

    LOGGER.info("Initializing MATLAB engine")
    eng = engine.start_matlab()
    LOGGER.debug("Loading project file")
    eng.matlab.project.loadProject(MATLAB_PROJECTFILE)
    LOGGER.debug("Loading Simulink model")
    mdl = LAB_SLX_FILENAME
    simin = eng.py_load_model(mdl, time_delta, end_time, time, power, amb_temp)

    LOGGER.info("Estimating parameters for experiment without air")
    *_, params = perform_experiment(
        data,
        eng,
        mdl,
        simin,
    )
    with open(
        os.path.join("examples", "python", "lab_240716", "estimated_parameters.json"),
        "w",
    ) as f:
        f.write(json.dumps(params._asdict()))
