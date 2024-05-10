from scipy import optimize
import numpy as np

from thermal_model.estimation.params import TargetParams
from thermal_model.estimation.models import generate_l2_error, target_lti_parameters, get_lti_params_fn

_GOOD_DEFAULT_PARAMS = TargetParams(cp=2288.8086878520617, cair=40.68543129463231,
                                    rair=0.05622811486407936, rin=0.29153746960754423, rout=0.09544187302807855)
_EPSILON = 1e-9


def lti_from_data(y, u, t, x0, chamber_std, cell_std, initial_guess=None,
                  system_kwargs=None, optimizer_kwargs=None,
                  optimizer_fn=optimize.minimize) -> tuple[
                      tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray],
                      TargetParams
]:
    if initial_guess is None:
        initial_guess = _GOOD_DEFAULT_PARAMS
    if system_kwargs is None:
        system_kwargs = {}
    if optimizer_kwargs is None:
        optimizer_kwargs = {}

    error_fn = generate_l2_error(
        y, u, t, x0, chamber_std, cell_std, **system_kwargs)

    if "bounds" not in optimizer_kwargs:
        optimizer_kwargs["bounds"] = optimize.Bounds(_EPSILON, np.inf)
    if "jac" not in optimizer_kwargs:
        optimizer_kwargs["jac"] = "3-point"
    if "hess" not in optimizer_kwargs:
        optimizer_kwargs["hess"] = "3-point"
    if "method" not in optimizer_kwargs:
        optimizer_kwargs["method"] = "Nelder-Mead"
    if "options" not in optimizer_kwargs:
        optimizer_kwargs["options"] = {
            "disp": True,
        }

    params = optimizer_fn(
        lambda p: error_fn(TargetParams(*p)),
        np.array([*initial_guess]),
        **optimizer_kwargs,
    )
    final_params = TargetParams(*params.x)
    if "outputs" in system_kwargs:
        params_fn = get_lti_params_fn(system_kwargs["outputs"])
    else:
        params_fn = target_lti_parameters
    return params_fn(final_params), final_params
