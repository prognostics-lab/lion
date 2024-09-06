import numpy as np
from scipy import optimize
from thermal_model.estimation import error
from thermal_model.estimation.models import (get_lti_params_fn,
                                             target_lti_parameters)
from thermal_model.estimation.params import TargetParams, params_prefilled
from thermal_model.logger import LOGGER

_GOOD_DEFAULT_PARAMS = TargetParams(
    cp=2288.8086878520617,
    cair=40.68543129463231,
    rair=0.05622811486407936,
    rin=0.29153746960754423,
    rout=0.09544187302807855,
)
_EPSILON = 1e-12


def generate_costfn(errorfn, fixed_params):
    def f(p):
        return errorfn(TargetParams(*params_prefilled(p, fixed_params)))

    return f


class CostFunction:
    def __init__(self, errorfn, fixed_params):
        self.errorfn = errorfn
        self.fixed_params = fixed_params

    def __call__(self, p):
        return self.errorfn(TargetParams(*params_prefilled(p, self.fixed_params)))


def lti_from_data(
    y,
    u,
    t,
    x0,
    initial_guess=None,
    *,
    fixed_params=None,
    system_kwargs=None,
    optimizer_kwargs=None
) -> tuple[tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray], TargetParams]:
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

    if "fn" not in optimizer_kwargs:
        optimizer_fn = optimize.minimize
    else:
        optimizer_fn = optimizer_kwargs.pop("fn")
    LOGGER.debug("Using '%s' as optimizer fn", optimizer_fn.__name__)

    if "err" not in optimizer_kwargs:
        error_cls = error.ErrorL2
    else:
        error_cls = optimizer_kwargs.pop("err")
    LOGGER.debug("Using '%s' as error cls", error_cls.__name__)

    error_fn = error_cls(y, u, t, x0, **system_kwargs)

    LOGGER.info("Starting optimization process (maybe go make some coffee?)")
    params = optimizer_fn(
        # lambda p: error_fn(TargetParams(*params_prefilled(p, fixed_params))),
        CostFunction(error_fn, fixed_params),
        np.array([*initial_guess]),
        **{
            key: val
            for key, val in optimizer_kwargs.items()
            if key in optimizer_fn.__code__.co_varnames
        }
    )
    LOGGER.info("Finished optimization process")
    final_params = TargetParams(*params_prefilled(params.x, fixed_params))
    if "outputs" in system_kwargs:
        params_fn = get_lti_params_fn(system_kwargs["outputs"])
    else:
        params_fn = get_lti_params_fn()
    return params_fn(final_params), final_params
