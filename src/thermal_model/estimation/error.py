from typing import Callable

import numpy as np

from thermal_model.estimation.models import target_response
from thermal_model.estimation.params import TargetParams


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


def likelihood(expected, u, t, x0, sensor_cov, prior: Callable[[TargetParams], float] = None, **kwargs) -> Callable[[TargetParams], float]:
    # if prior is None:
    #     prior = lambda params: 1
    # sensor_cov_det = np.linalg.det(sensor_cov)
    # sensor_cov_inv = np.linalg.inv(sensor_cov)
    # def _generate_system(params: TargetParams) -> float:
    #     y = target_response(u, t, x0, params, **kwargs)
    #     error = expected - y
    #     rows, cols = error.shape
    #     prod = 1
    #     for e in error:
    #         sqrt_term = (2 * np.pi)**cols * sensor_cov_det
    #         prod *= np.exp(-0.5 * e @ sensor_cov_inv @ e.T) / np.sqrt(sqrt_term)
    #     return 1 - prod * prior(params)
    # return _generate_system
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

