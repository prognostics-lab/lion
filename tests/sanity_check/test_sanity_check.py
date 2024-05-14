import json
import os

import numpy as np
import pandas as pd
from scipy import optimize
import pytest

from thermal_model.estimation import lti_from_data, TargetParams
from thermal_model.estimation.models import generate_evaluation
from thermal_model.logger import LOGGER, setup_logger


@pytest.fixture
def acceptable_mse():
    return 1e-3


@pytest.fixture
def epsilon():
    return 1e-6


def test_fit(acceptable_mse, epsilon):
    # Get the data
    df = pd.read_csv(os.path.join("tests", "sanity_check", "sim.csv"))
    with open(os.path.join("tests", "sanity_check", "params.json"), "r") as file:
        real_params = json.load(file)

    y = np.array([df["sf_temp"].to_numpy(), df["air_temp"].to_numpy()]).T
    u = np.array([df["amb_temp"].to_numpy(), df["q_gen"].to_numpy()]).T
    x0 = np.array([real_params["in_temp"], real_params["air_temp"]])
    t = df["time"].to_numpy()

    (A, B, C, _), params = lti_from_data(
        y,
        u,
        t,
        x0,
        1e-1,
        0,
        optimizer_kwargs={
            # "method": "Nelder-Mead",
            # "callback": optimizer_callback,
            # "fn": optimize.minimize,
            "xtol": 1e-12,
            "method": "trf",
            "verbose": 2,
            "fn": optimize.least_squares,
        },
    )
    LOGGER.info("\n")
    LOGGER.info(f"Final parameters: {params}")
    LOGGER.info(f"A = \n{A}")
    LOGGER.info(f"B = \n{B}")
    LOGGER.info(f"C = \n{C}")
    print(params)

    assert np.linalg.matrix_rank(np.vstack([C, C @ A])) == 2

    evaluate = generate_evaluation(y, u, t, x0)
    *_, error, _ = evaluate(params)
    try:
        mse = np.diag(error.conjugate().T @ error).sum() / len(error)
    except ValueError:
        mse = error.conjugate().T @ error / len(error)

    assert np.abs(mse) <= acceptable_mse
    for key, val in params._asdict().items():
        assert np.abs(val - real_params[key]) <= epsilon
