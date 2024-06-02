from scipy import optimize
import numpy as np

from thermal_model.estimation.params import TargetParams, params_prefilled
from thermal_model.estimation.models import target_lti_parameters, get_lti_params_fn
from thermal_model.estimation import error

_GOOD_DEFAULT_PARAMS = TargetParams(cp=2288.8086878520617, cair=40.68543129463231,
                                    rair=0.05622811486407936, rin=0.29153746960754423, rout=0.09544187302807855)
_EPSILON = 1e-12


def lti_from_data(y, u, t, x0, initial_guess=None, *, fixed_params=None,
                  system_kwargs=None, optimizer_kwargs=None) -> tuple[
                      tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray],
                      TargetParams
]:
    if fixed_params is None:
        fixed_params = {}
    if initial_guess is None:
        initial_guess = _GOOD_DEFAULT_PARAMS
    if system_kwargs is None:
        system_kwargs = {}
    if optimizer_kwargs is None:
        optimizer_kwargs = {}

    if "bounds" not in optimizer_kwargs:
        optimizer_kwargs["bounds"] = optimize.Bounds(_EPSILON, np.inf)
    # if "jac" not in optimizer_kwargs:
    #     optimizer_kwargs["jac"] = "3-point"
    # if "hess" not in optimizer_kwargs:
    #     optimizer_kwargs["hess"] = "3-point"
    # if "method" not in optimizer_kwargs:
    #     optimizer_kwargs["method"] = "Nelder-Mead"
    # if "options" not in optimizer_kwargs:
    #     optimizer_kwargs["options"] = {
    #         "disp": True,
    #     }

    if "fn" not in optimizer_kwargs:
        optimizer_fn = optimize.minimize
    else:
        optimizer_fn = optimizer_kwargs.pop("fn")

    if "err" not in optimizer_kwargs:
        error_functional = error.l2
    else:
        error_functional = optimizer_kwargs.pop("err")

    error_fn = error_functional(
        y, u, t, x0, **system_kwargs)

    params = optimizer_fn(
        lambda p: error_fn(TargetParams(*params_prefilled(p, fixed_params))),
        np.array([*initial_guess]),
        **{
            key: val
            for key, val in optimizer_kwargs.items()
            if key in optimizer_fn.__code__.co_varnames
        }
    )
    final_params = TargetParams(*params_prefilled(params.x, fixed_params))
    if "outputs" in system_kwargs:
        params_fn = get_lti_params_fn(system_kwargs["outputs"])
    else:
        params_fn = get_lti_params_fn()
    return params_fn(final_params), final_params
