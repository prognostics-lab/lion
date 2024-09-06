import abc
from typing import Callable

import numpy as np
from thermal_model.estimation.models import target_response
from thermal_model.estimation.params import TargetParams
from thermal_model.logger import LOGGER


class ErrorFunction(abc.ABC):
    def __call__(self, params: TargetParams) -> float:
        return self.eval(params)

    @abc.abstractmethod
    def eval(self, params: TargetParams) -> float:
        """Evaluation function"""


class ErrorL1(ErrorFunction):
    """L1 error function (mean absolute error)"""

    def __init__(self, expected, u, t, x0, **kwargs):
        self.expected = expected
        self.u = u
        self.t = t
        self.x0 = x0
        self.kwargs = kwargs

    def eval(self, params: TargetParams) -> float:
        obtained = target_response(self.u, self.t, self.x0, params, **self.kwargs)
        error = self.expected - obtained
        return np.abs(error).sum() / len(self.t)


class ErrorL2(ErrorFunction):
    """L2 error function (mean square error)"""

    def __init__(self, expected, u, t, x0, **kwargs):
        self.expected = expected
        self.u = u
        self.t = t
        self.x0 = x0
        self.kwargs = kwargs

    def eval(self, params: TargetParams) -> float:
        obtained = target_response(self.u, self.t, self.x0, params, **self.kwargs)
        error = self.expected - obtained
        try:
            mse = np.diag(error.conjugate().T @ error).sum() / len(self.t)
        except ValueError:
            mse = error.conjugate().T @ error / len(self.t)
        return mse


class ErrorL2Simulation(ErrorFunction):
    """L2 error function (mean square error) from simulation"""

    def __init__(
        self,
        expected,
        u,
        t,
        x0,
        engine,
        mdl,
        simin,
        initial_soc,
        internal_resistance=0,
        nominal_capacity=0,
        gain=1,
        **kwargs,
    ):
        self.expected = expected
        self.u = u
        self.t = t
        self.x0 = x0
        self.engine = engine
        self.mdl = mdl
        self.simin = simin
        self.initial_soc = initial_soc
        self.internal_resistance = internal_resistance
        self.nominal_capacity = nominal_capacity
        self.gain = gain
        self.kwargs = kwargs

        self.names = self.kwargs.get("outputs")
        only_sf = self.names is not None and self.names != "both"
        self.initial_conditions_dict = {
            "initial_soc": self.initial_soc,
        }
        if only_sf:
            self.initial_conditions_dict["initial_in_temp"] = self.x0
            self.expected = self.expected[1:]
        else:
            self.initial_conditions_dict["initial_in_temp"] = self.x0[0]
            self.initial_conditions_dict["initial_air_temp"] = self.x0[1]

        self.constant_params = {
            "internal_resistance": self.internal_resistance,
            "nominal_capacity": self.nominal_capacity,
        }

    def eval(self, params: TargetParams) -> float:
        LOGGER.debug("Calling simulation")
        params_dict = params._asdict()
        simout = self.engine.py_evaluate_model(
            self.mdl,
            self.simin,
            params_dict,
            self.initial_conditions_dict,
            self.constant_params,
        )
        obtained = np.array(simout)[:, 2]
        LOGGER.debug("Calculating error")
        error = self.expected - obtained
        try:
            mse = np.diag(error.conjugate().T @ error).sum() / len(self.t)
        except ValueError:
            mse = error.conjugate().T @ error / len(self.t)
        print(f"MSE = {mse}")
        return self.gain * mse


class ErrorLikelihood(ErrorFunction):
    """Likelihood based error function"""

    def __init__(
        self,
        expected,
        u,
        t,
        x0,
        sensor_cov,
        prior: Callable[[TargetParams], float] | None = None,
        **kwargs,
    ):
        self.expected = expected
        self.u = u
        self.t = t
        self.x0 = x0
        self.sensor_cov = sensor_cov
        self.prior = prior
        self.kwargs = kwargs

    def eval(self, params: TargetParams) -> float:
        _prior = 1.0
        if self.prior is not None:
            _prior = self.prior(params)

        if len(self.x0) != 1:
            sensor_cov_det = np.linalg.det(self.sensor_cov)
            sensor_cov_inv = np.linalg.inv(self.sensor_cov)

            y = target_response(self.u, self.t, self.x0, params, **self.kwargs)
            error = self.expected - y
            rows, cols = error.shape
            exp_term = np.exp(-0.5 * np.diag(error.T @ error @ sensor_cov_inv).sum())
            # sqrt_term = ((2 * np.pi) ** cols * sensor_cov_det) ** rows
            sqrt_term = 1
            return -_prior * exp_term / np.sqrt(sqrt_term)

        else:
            sensor_cov_det = np.abs(self.sensor_cov)

            y = target_response(self.u, self.t, self.x0, params, **self.kwargs)
            error = self.expected - y
            exp_term = np.exp(-(error**2) / (2 * sensor_cov_det))
            sqrt_term = np.sqrt(2 * np.pi * sensor_cov_det)
            return -_prior * (exp_term / sqrt_term).prod()


# @deprecated("Use class-based error functions instead")
# def l1(expected, u, t, x0, **kwargs) -> Callable[[TargetParams], float]:
#     def _generate_system(params: TargetParams) -> float:
#         obtained = target_response(u, t, x0, params, **kwargs)
#         error = expected - obtained
#         return np.abs(error).sum() / len(t)

#     return _generate_system


# @deprecated("Use class-based error functions instead")
# def l2(expected, u, t, x0, **kwargs) -> Callable[[TargetParams], float]:
#     def _generate_system(params: TargetParams) -> float:
#         obtained = target_response(u, t, x0, params, **kwargs)
#         error = expected - obtained
#         try:
#             mse = np.diag(error.conjugate().T @ error).sum() / len(t)
#         except ValueError:
#             mse = error.conjugate().T @ error / len(t)
#         return mse

#     return _generate_system


# @deprecated("Use class-based error functions instead")
# def l2_simulation(
#     expected,
#     u,
#     t,
#     x0,
#     engine,
#     mdl,
#     simin,
#     initial_soc,
#     internal_resistance=0,
#     nominal_capacity=0,
#     gain=1,
#     **kwargs,
# ) -> Callable[[TargetParams], float]:
#     names = kwargs.get("outputs")
#     only_sf = names is not None and names != "both"
#     initial_conditions_dict = {
#         "initial_soc": initial_soc,
#     }
#     if only_sf:
#         initial_conditions_dict["initial_in_temp"] = x0
#         expected = expected[1:]
#     else:
#         initial_conditions_dict["initial_in_temp"] = x0[0]
#         initial_conditions_dict["initial_air_temp"] = x0[1]

#     constant_params = {
#         "internal_resistance": internal_resistance,
#         "nominal_capacity": nominal_capacity,
#     }

#     def _generate_system(params: TargetParams) -> float:
#         LOGGER.debug("Calling simulation")
#         params_dict = params._asdict()
#         simout = engine.py_evaluate_model(
#             mdl, simin, params_dict, initial_conditions_dict, constant_params
#         )
#         obtained = np.array(simout)[:, 2]
#         LOGGER.debug("Calculating error")
#         error = expected - obtained
#         try:
#             mse = np.diag(error.conjugate().T @ error).sum() / len(t)
#         except ValueError:
#             mse = error.conjugate().T @ error / len(t)
#         print(f"MSE = {mse}")
#         return gain * mse

#     return _generate_system


# @deprecated("Logarithmic error functions not supported anymore")
# def logl2(*args, **kwargs) -> Callable[[TargetParams], float]:
#     out_fn = l2(*args, **kwargs)
#     return lambda x: np.log(out_fn(x))


# @deprecated("Logarithmic error functions not supported anymore")
# def logl2_simulation(*args, **kwargs) -> Callable[[TargetParams], float]:
#     out_fn = l2_simulation(*args, **kwargs)
#     return lambda x: np.log(out_fn(x))


# @deprecated("Use class-based error functions instead")
# def likelihood(
#     expected,
#     u,
#     t,
#     x0,
#     sensor_cov,
#     prior: Callable[[TargetParams], float] | None = None,
#     **kwargs,
# ) -> Callable[[TargetParams], float]:
#     if prior is None:
#         prior = lambda params: 1
#     if len(x0) != 1:
#         sensor_cov_det = np.linalg.det(sensor_cov)
#         sensor_cov_inv = np.linalg.inv(sensor_cov)

#         def _generate_system(params: TargetParams) -> float:
#             y = target_response(u, t, x0, params, **kwargs)
#             error = expected - y
#             rows, cols = error.shape
#             exp_term = np.exp(-0.5 * np.diag(error.T @ error @ sensor_cov_inv).sum())
#             # sqrt_term = ((2 * np.pi) ** cols * sensor_cov_det) ** rows
#             sqrt_term = 1
#             return -prior(params) * exp_term / np.sqrt(sqrt_term)

#     else:
#         sensor_cov_det = np.abs(sensor_cov)

#         def _generate_system(params: TargetParams) -> float:
#             y = target_response(u, t, x0, params, **kwargs)
#             error = expected - y
#             exp_term = np.exp(-(error**2) / (2 * sensor_cov_det))
#             sqrt_term = np.sqrt(2 * np.pi * sensor_cov_det)
#             return -prior(params) * (exp_term / sqrt_term).prod()

#     return _generate_system


# @deprecated("Use class-based error functions instead")
# def likelihood_simulation(
#     expected,
#     u,
#     t,
#     x0,
#     engine,
#     mdl,
#     simin,
#     initial_soc,
#     sensor_cov,
#     prior: Callable[[TargetParams], float] | None = None,
#     **kwargs,
# ) -> Callable[[TargetParams], float]:
#     names = kwargs.get("outputs")
#     only_sf = names is not None and names != "both"
#     initial_conditions_dict = {
#         "initial_soc": initial_soc,
#     }
#     if only_sf:
#         initial_conditions_dict["initial_in_temp"] = x0
#         expected = expected[1:]
#     else:
#         initial_conditions_dict["initial_in_temp"] = x0[0]
#         initial_conditions_dict["initial_air_temp"] = x0[1]
#     if prior is None:
#         prior = lambda params: 1
#     if len(x0) != 1:
#         sensor_cov_det = np.linalg.det(sensor_cov)
#         sensor_cov_inv = np.linalg.inv(sensor_cov)

#         def _generate_system(params: TargetParams) -> float:
#             params_dict = params._asdict()
#             simout = engine.py_evaluate_model(
#                 mdl, simin, params_dict, initial_conditions_dict
#             )
#             obtained = np.array(simout)[:, 2]
#             error = expected - obtained
#             rows, cols = error.shape
#             exp_term = np.exp(-0.5 * np.diag(error.T @ error @ sensor_cov_inv).sum())
#             # sqrt_term = ((2 * np.pi) ** cols * sensor_cov_det) ** rows
#             sqrt_term = 1
#             return -prior(params) * exp_term / np.sqrt(sqrt_term)

#     else:
#         sensor_cov_det = np.abs(sensor_cov)

#         def _generate_system(params: TargetParams) -> float:
#             params_dict = params._asdict()
#             simout = engine.py_evaluate_model(
#                 mdl, simin, params_dict, initial_conditions_dict
#             )
#             obtained = np.array(simout)[:, 2]
#             error = expected - obtained
#             exp_term = np.exp(-(error**2) / (2 * sensor_cov_det))
#             sqrt_term = np.sqrt(2 * np.pi * sensor_cov_det)
#             return -prior(params) * (exp_term / sqrt_term).prod()

#     return _generate_system
