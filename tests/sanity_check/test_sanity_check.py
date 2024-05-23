import json
import os

import numpy as np
import pandas as pd
from scipy import optimize
import pytest

from thermal_model.estimation import lti_from_data
from thermal_model.estimation.models import generate_evaluation
from thermal_model.logger import LOGGER, setup_logger


_EPSILON = 1e-9


@pytest.fixture(scope="session", autouse=True)
def logger():
    setup_logger(quiet=False, debug=True, verbose=True)
    return LOGGER


@pytest.fixture(scope="session", autouse=True)
def acceptable_mse():
    return 1e-3


@pytest.fixture(scope="session", autouse=True)
def alpha():
    return 0.05


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
def optimize_results(logger, data):
    y, u, t, x0, _ = data

    (A, B, C, _), params = lti_from_data(
        y,
        u,
        t,
        x0,
        0,
        0,
        optimizer_kwargs={
            "fn": optimize.minimize,
            "method": "Nelder-Mead",
            "tol": 1e-3,
            "options": {
                "maxiter": 1e2,
            },
            # "fn": optimize.least_squares,
            # "method": "trf",
            # "verbose": 2,
            # "bounds": (_EPSILON, np.inf),
        },
    )
    logger.info("\n")
    logger.info(f"Final parameters: {params}")
    logger.info(f"A = \n{A}")
    logger.info(f"B = \n{B}")
    logger.info(f"C = \n{C}")
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


def test_cp(optimize_results, alpha):
    *_, params, (*_, real_params) = optimize_results
    print(f"Real parameters: {real_params}")
    print(f"Parameters: {params}")
    percentual_error = np.abs(params.cp - real_params["cp"]) / real_params["cp"]
    assert percentual_error <= alpha, f"Parameter 'cp' is below acceptable (ERR% is {
        100 * percentual_error:.2f}% > {100 * alpha:.0f}%)"


def test_cair(optimize_results, alpha):
    *_, params, (*_, real_params) = optimize_results
    print(f"Real parameters: {real_params}")
    print(f"Parameters: {params}")
    percentual_error = np.abs(params.cair - real_params["cair"]) / real_params["cair"]
    assert percentual_error <= alpha, f"Parameter 'cair' is below acceptable (ERR% is {
        100 * percentual_error:.2f}% > {100 * alpha:.0f}%)"


def test_rin(optimize_results, alpha):
    *_, params, (*_, real_params) = optimize_results
    print(f"Real parameters: {real_params}")
    print(f"Parameters: {params}")
    percentual_error = np.abs(params.rin - real_params["rin"]) / real_params["rin"]
    assert percentual_error <= alpha, f"Parameter 'rin' is below acceptable (ERR% is {
        100 * percentual_error:.2f}% > {100 * alpha:.0f}%)"


def test_rout(optimize_results, alpha):
    *_, params, (*_, real_params) = optimize_results
    print(f"Real parameters: {real_params}")
    print(f"Parameters: {params}")
    percentual_error = np.abs(params.rout - real_params["rout"]) / real_params["rout"]
    assert percentual_error <= alpha, f"Parameter 'rout' is below acceptable (ERR% is {
        100 * percentual_error:.2f}% > {100 * alpha:.0f}%)"


def test_rair(optimize_results, alpha):
    *_, params, (*_, real_params) = optimize_results
    print(f"Real parameters: {real_params}")
    print(f"Parameters: {params}")
    percentual_error = np.abs(params.rair - real_params["rair"]) / real_params["rair"]
    assert percentual_error <= alpha, f"Parameter 'rair' is below acceptable (ERR% is {
        100 * percentual_error:.2f}% > {100 * alpha:.0f}%)"
