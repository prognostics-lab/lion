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


def likelihood_no_model_noise(expected, u, t, x0, sensor_cov, prior=None, **kwargs) -> Callable[[TargetParams], float]:
    if prior is None:
        prior = lambda params: 1
    sensor_cov_det = np.linalg.det(sensor_cov)
    sensor_cov_inv = np.linalg.inv(sensor_cov)
    def _generate_system(params: TargetParams) -> float:
        y = target_response(u, t, x0, params, **kwargs)
        error = expected - y
        rows, cols = error.shape
        exp_term = np.exp(-0.5 * np.diag(error.T @ error @ sensor_cov_inv).sum())
        # sqrt_term = ((2 * np.pi) ** cols * sensor_cov_det) ** rows
        sqrt_term = 1
        return -(1 / np.sqrt(sqrt_term)) * exp_term * prior(params)
    return _generate_system

