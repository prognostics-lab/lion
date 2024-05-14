import json
import os

import numpy as np
import pandas as pd
from scipy import optimize
import pytest

from thermal_model.estimation import lti_from_data, TargetParams
from thermal_model.estimation.models import generate_evaluation
from thermal_model.logger import LOGGER, setup_logger


@pytest.fixture(scope="session", autouse=True)
def acceptable_mse():
    return 1e-3


@pytest.fixture(scope="session", autouse=True)
def epsilon():
    return 1e-6


@pytest.fixture(scope="session", autouse=True)
def data():
    df = pd.read_csv(os.path.join("tests", "sanity_check", "sim.csv"))
    with open(os.path.join("tests", "sanity_check", "params.json"), "r") as file:
        real_params = json.load(file)

    y = np.array([df["sf_temp"].to_numpy(), df["air_temp"].to_numpy()]).T
    u = np.array([df["amb_temp"].to_numpy(), df["q_gen"].to_numpy()]).T
    t = df["time"].to_numpy()
    x0 = np.array([real_params["in_temp"], real_params["air_temp"]])
    return y, u, t, x0, real_params


@pytest.fixture(scope="session", autouse=True)
def optimize_results(data):
    y, u, t, x0, real_params = data

    (A, B, C, _), params = lti_from_data(
        y,
        u,
        t,
        x0,
        0,
        0,
        optimizer_kwargs={
            "method": "Nelder-Mead",
            "tol": 1e-18,
            "fn": optimize.minimize,
            "options": {
                "maxiter": 1e1,
            },
            # "method": "trf",
            # "verbose": 2,
            # "bounds": (_EPSILON, np.inf),
            # "fn": optimize.least_squares,
        },
    )
    LOGGER.info("\n")
    LOGGER.info(f"Final parameters: {params}")
    LOGGER.info(f"A = \n{A}")
    LOGGER.info(f"B = \n{B}")
    LOGGER.info(f"C = \n{C}")
    print(params)
    return A, B, C, params, data


def test_observable(optimize_results):
    A, _, C, *_ = optimize_results
    assert np.linalg.matrix_rank(
        np.vstack([C, C @ A])) == 2, "System is not observable"


def test_mse(optimize_results, acceptable_mse):
    *_, params, (y, u, t, x0, _) = optimize_results
    evaluate = generate_evaluation(y, u, t, x0)
    *_, error, _ = evaluate(params)
    try:
        mse = np.diag(error.conjugate().T @ error).sum() / len(error)
    except ValueError:
        mse = error.conjugate().T @ error / len(error)
    abs_mse = np.abs(mse)
    assert abs_mse <= acceptable_mse, f"MSE is below acceptable ({abs_mse:.4e} > {
        acceptable_mse:.4e})"


def test_cp(optimize_results, epsilon):
    *_, params, (*_, real_params) = optimize_results
    abs_error = np.abs(params.cp - real_params["cp"])
    assert abs_error <= epsilon, f"Parameter 'cp' is below acceptable ({
        abs_error:.4e} > {epsilon:.4e})"


def test_cair(optimize_results, epsilon):
    *_, params, (*_, real_params) = optimize_results
    abs_error = np.abs(params.cair - real_params["cair"])
    assert abs_error <= epsilon, f"Parameter 'cair' is below acceptable ({
        abs_error:.4e} > {epsilon:.4e})"


def test_rin(optimize_results, epsilon):
    *_, params, (*_, real_params) = optimize_results
    abs_error = np.abs(params.rin - real_params["rin"])
    assert abs_error <= epsilon, f"Parameter 'rin' is below acceptable ({
        abs_error:.4e} > {epsilon:.4e})"


def test_rout(optimize_results, epsilon):
    *_, params, (*_, real_params) = optimize_results
    abs_error = np.abs(params.rout - real_params["rout"])
    assert abs_error <= epsilon, f"Parameter 'rout' is below acceptable ({
        abs_error:.4e} > {epsilon:.4e})"


def test_rair(optimize_results, epsilon):
    *_, params, (*_, real_params) = optimize_results
    abs_error = np.abs(params.rair - real_params["rair"])
    assert abs_error <= epsilon, f"Parameter 'rair' is below acceptable ({
        abs_error:.4e} > {epsilon:.4e})"
