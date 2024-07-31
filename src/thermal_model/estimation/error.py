from typing import Callable

import numpy as np

from thermal_model.estimation.models import target_response
from thermal_model.estimation.params import TargetParams
from thermal_model.logger import LOGGER


def l1(expected, u, t, x0, **kwargs) -> Callable[[TargetParams], float]:
    def _generate_system(params: TargetParams) -> float:
        obtained = target_response(u, t, x0, params, **kwargs)
        error = expected - obtained
        return np.abs(error).sum() / len(t)
    return _generate_system


def l2(expected, u, t, x0, **kwargs) -> Callable[[TargetParams], float]:
    def _generate_system(params: TargetParams) -> float:
        obtained = target_response(u, t, x0, params, **kwargs)
        error = expected - obtained
        try:
            mse = np.diag(error.conjugate().T @ error).sum() / len(t)
        except ValueError:
            mse = error.conjugate().T @ error / len(t)
        return mse
    return _generate_system


def l2_simulation(expected, u, t, x0, engine, mdl, simin, initial_soc, gain=1, **kwargs) -> Callable[[TargetParams], float]:
    names = kwargs.get("outputs")
    only_sf = names is not None and names != "both"
    initial_conditions_dict = {
        "initial_soc": initial_soc,
    }
    if only_sf:
        initial_conditions_dict["initial_in_temp"] = x0
        expected = expected[1:]
    else:
        initial_conditions_dict["initial_in_temp"] = x0[0]
        initial_conditions_dict["initial_air_temp"] = x0[1]

    def _generate_system(params: TargetParams) -> float:
        LOGGER.debug("Calling simulation")
        params_dict = params._asdict()
        simout = engine.py_evaluate_model(mdl, simin, params_dict, initial_conditions_dict)
        obtained = np.array(simout)[:, 2]
        LOGGER.debug("Calculating error")
        error = expected - obtained
        print(expected)
        print(obtained)
        try:
            mse = np.diag(error.conjugate().T @ error).sum() / len(t)
        except ValueError:
            mse = error.conjugate().T @ error / len(t)
        print(mse)
        return gain * mse
    return _generate_system


def logl2(*args, **kwargs) -> Callable[[TargetParams], float]:
    out_fn = l2(*args, **kwargs)
    return lambda x: np.log(out_fn(x))


def logl2_simulation(*args, **kwargs) -> Callable[[TargetParams], float]:
    out_fn = l2_simulation(*args, **kwargs)
    return lambda x: np.log(out_fn(x))


def likelihood(expected, u, t, x0, sensor_cov, prior: Callable[[TargetParams], float] = None, **kwargs) -> Callable[[TargetParams], float]:
    if prior is None:
        prior = lambda params: 1
    if len(x0) != 1:
        sensor_cov_det = np.linalg.det(sensor_cov)
        sensor_cov_inv = np.linalg.inv(sensor_cov)
        def _generate_system(params: TargetParams) -> float:
            y = target_response(u, t, x0, params, **kwargs)
            error = expected - y
            rows, cols = error.shape
            exp_term = np.exp(-0.5 * np.diag(error.T @ error @ sensor_cov_inv).sum())
            # sqrt_term = ((2 * np.pi) ** cols * sensor_cov_det) ** rows
            sqrt_term = 1
            return -prior(params) * exp_term  / np.sqrt(sqrt_term)
    else:
        sensor_cov_det = np.abs(sensor_cov)
        def _generate_system(params: TargetParams) -> float:
            y = target_response(u, t, x0, params, **kwargs)
            error = expected - y
            exp_term = np.exp(-error**2 / (2 * sensor_cov_det))
            sqrt_term = np.sqrt(2 * np.pi * sensor_cov_det)
            return -prior(params) * (exp_term / sqrt_term).prod()
    return _generate_system


def likelihood_simulation(expected, u, t, x0, engine, mdl, simin, initial_soc, sensor_cov, prior: Callable[[TargetParams], float] = None, **kwargs) -> Callable[[TargetParams], float]:
    names = kwargs.get("outputs")
    only_sf = names is not None and names != "both"
    initial_conditions_dict = {
        "initial_soc": initial_soc,
    }
    if only_sf:
        initial_conditions_dict["initial_in_temp"] = x0
        expected = expected[1:]
    else:
        initial_conditions_dict["initial_in_temp"] = x0[0]
        initial_conditions_dict["initial_air_temp"] = x0[1]
    if prior is None:
        prior = lambda params: 1
    if len(x0) != 1:
        sensor_cov_det = np.linalg.det(sensor_cov)
        sensor_cov_inv = np.linalg.inv(sensor_cov)
        def _generate_system(params: TargetParams) -> float:
            params_dict = params._asdict()
            simout = engine.py_evaluate_model(mdl, simin, params_dict, initial_conditions_dict)
            obtained = np.array(simout)[:, 2]
            error = expected - obtained
            rows, cols = error.shape
            exp_term = np.exp(-0.5 * np.diag(error.T @ error @ sensor_cov_inv).sum())
            # sqrt_term = ((2 * np.pi) ** cols * sensor_cov_det) ** rows
            sqrt_term = 1
            return -prior(params) * exp_term  / np.sqrt(sqrt_term)
    else:
        sensor_cov_det = np.abs(sensor_cov)
        def _generate_system(params: TargetParams) -> float:
            params_dict = params._asdict()
            simout = engine.py_evaluate_model(mdl, simin, params_dict, initial_conditions_dict)
            obtained = np.array(simout)[:, 2]
            error = expected - obtained
            exp_term = np.exp(-error**2 / (2 * sensor_cov_det))
            sqrt_term = np.sqrt(2 * np.pi * sensor_cov_det)
            return -prior(params) * (exp_term / sqrt_term).prod()
    return _generate_system
