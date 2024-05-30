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


def likelihood(expected, u, t, x0, sensor_std, **kwargs) -> Callable[[TargetParams], float]:
    def _generate_system(params: TargetParams) -> float:
        raise NotImplementedError("ML estimation not currently implemented")
    return _generate_system

