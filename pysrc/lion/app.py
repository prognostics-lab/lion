from enum import Enum
from collections import namedtuple
from functools import singledispatchmethod
from typing import Callable
import logging

import numpy as np

import lion_ffi as _
from _lion import ffi
from _lion import lib as _lionl
from lion import dtypes
from lion.exceptions import LionException
from lion.status import Status, ffi_call
from lion.app_config import Stepper, Regime, Minimizer
from lion.vector import Vector, Vectorizable
from lion_utils.logger import LOGGER


def _generate_init_pythoncb(func: Callable[["App"], Status]):
    @ffi.def_extern()
    def init_pythoncb(app):
        return func(app)

    return init_pythoncb


def _generate_update_pythoncb(func: Callable[["App"], Status]):
    @ffi.def_extern()
    def update_pythoncb(app):
        return func(app)

    return update_pythoncb


def _generate_finished_pythoncb(func: Callable[["App"], Status]):
    @ffi.def_extern()
    def finished_pythoncb(app):
        return func(app)

    return finished_pythoncb


class LogLvl(Enum):
    TRACE = _lionl.LOG_TRACE
    DEBUG = _lionl.LOG_DEBUG
    INFO = _lionl.LOG_INFO
    WARN = _lionl.LOG_WARN
    ERROR = _lionl.LOG_ERROR
    FATAL = _lionl.LOG_FATAL


def lvl_from_logger() -> LogLvl | None:
    lvl = logging.INFO
    for h in LOGGER.handlers:
        if h.get_name() == "stderr":
            lvl = h.level
            break

    if lvl == logging.DEBUG:
        out = LogLvl.DEBUG
    elif lvl == logging.INFO:
        out = LogLvl.INFO
    elif lvl == logging.WARN:
        out = LogLvl.WARN
    elif lvl == logging.ERROR:
        out = LogLvl.ERROR
    elif lvl == logging.CRITICAL:
        out = LogLvl.FATAL
    else:
        out = None
    return out


class Config:
    """Lion application configuration"""

    __slots__ = ("_cdata",)

    def __init__(
        self,
        name=None,
        init=None,
        update=None,
        finished=None,
        regime: Regime | None = None,
        stepper: Stepper | None = None,
        minimizer: Minimizer | None = None,
        step: float | None = None,
        epsabs: float | None = None,
        epsrel: float | None = None,
        min_maxiter: int | None = None,
        log_stdlvl: LogLvl | None = None,
    ):
        self._cdata = ffi.new("lion_app_config_t *", _lionl.lion_app_config_default())

        if name is not None:
            self.name = name
        if init is not None:
            self.init = init
        if update is not None:
            self.update = update
        if finished is not None:
            self.finished = finished
        if regime is not None:
            self.sim_regime = regime
        if stepper is not None:
            self.sim_stepper = stepper
        if minimizer is not None:
            self.sim_minimizer = minimizer
        if step is not None:
            self.sim_step_seconds = step
        if epsabs is not None:
            self.sim_epsabs = epsabs
        if epsrel is not None:
            self.sim_epsrel = epsrel
        if min_maxiter is not None:
            self.sim_min_maxiter = min_maxiter

        if log_stdlvl is not None:
            self.log_stdlvl = log_stdlvl
        else:
            lvl = lvl_from_logger()
            if lvl is not None:
                self.log_stdlvl = lvl

    @property
    def name(self) -> str:
        return ffi.string(self._cdata.app_name)

    @name.setter
    def name(self, new_name: str):
        self._cdata.app_name = ffi.new("char[]", new_name)
        LOGGER.debug(f"New name: {ffi.string(self._cdata.app_name)}")

    @property
    def init(self) -> None:
        raise NotImplementedError("Can't fetch C functions")

    @init.setter
    def init(self, new_func: Callable[["App"], Status]):
        self._cdata.init_hook = _generate_init_pythoncb(new_func)

    @property
    def update(self) -> None:
        raise NotImplementedError("Can't fetch C functions")

    @update.setter
    def update(self, new_func: Callable[["App"], Status]):
        self._cdata.update_hook = _generate_update_pythoncb(new_func)

    @property
    def finished(self) -> None:
        raise NotImplementedError("Can't fetch C functions")

    @finished.setter
    def finished(self, new_func: Callable[["App"], Status]):
        self._cdata.finished_hook = _generate_finished_pythoncb(new_func)

    @property
    def sim_regime(self) -> Regime:
        return Regime(self._cdata.sim_regime)

    @sim_regime.setter
    def sim_regime(self, new_regime: Regime):
        self._cdata.sim_regime = new_regime.value

    @property
    def sim_stepper(self) -> Regime:
        return Stepper(self._cdata.sim_stepper)

    @sim_stepper.setter
    def sim_stepper(self, new_stepper: Stepper):
        self._cdata.sim_stepper = new_stepper.value

    @property
    def sim_minimizer(self) -> Minimizer:
        return Minimizer(self._cdata.sim_minimizer)

    @sim_minimizer.setter
    def sim_minimizer(self, new_minimizer: Minimizer):
        self._cdata.sim_minimizer = new_minimizer.value

    @property
    def sim_step_seconds(self) -> float:
        return self._cdata.sim_step_seconds

    @sim_step_seconds.setter
    def sim_step_seconds(self, new_time: float):
        self._cdata.sim_step_seconds = new_time

    @property
    def sim_epsabs(self) -> float:
        return self._cdata.sim_epsabs

    @sim_epsabs.setter
    def sim_epsabs(self, new_eps: float):
        self._cdata.sim_epsabs = new_eps

    @property
    def sim_epsrel(self) -> float:
        return self._cdata.sim_epsrel

    @sim_epsrel.setter
    def sim_epsrel(self, new_eps: float):
        self._cdata.sim_epsrel = new_eps

    @property
    def sim_min_maxiter(self) -> int:
        return self._cdata.sim_min_max_iter

    @sim_min_maxiter.setter
    def sim_min_maxiter(self, new_maxiter: int):
        self._cdata.sim_min_max_iter = new_maxiter

    @property
    def log_stdlvl(self) -> LogLvl:
        return LogLvl(self._cdata.log_stdlvl)

    @log_stdlvl.setter
    def log_stdlvl(self, new_lvl: LogLvl):
        self._cdata.log_stdlvl = new_lvl.value


ParamsInit = namedtuple("ParamsInit", "soc, soh, temp_in, capacity, current_guess")
ParamsEHC = namedtuple("ParamsEHC", "a, b, kappa, mu, sigma, l")
ParamsOCV = namedtuple("ParamsOCV", "alpha, beta, gamma, v0, vl")
ParamsVFT = namedtuple("ParamsVFT", "k1, k2, tref")
ParamsTemp = namedtuple("ParamsTemp", "cp, rin, rout")


class Params:
    """Lion application parameters"""

    __slots__ = ("_cdata",)

    def __init__(self):
        self._cdata = ffi.new("lion_params_t *", _lionl.lion_params_default())


class App:
    """Lion application to run"""

    __slots__ = ("_cdata", "config", "params")

    def __init__(self, config: Config | None = None, params: Params | None = None):
        self._cdata = ffi.new("lion_app_t *")
        if config is None:
            self.config = Config()
        else:
            self.config = config
        if params is None:
            self.params = Params()
        else:
            self.params = params

        _lionl.lion_app_new(self.config._cdata, self.params._cdata, self._cdata)

    def __del__(self):
        try:
            ffi_call(_lionl.lion_app_cleanup(self._cdata), "Failed cleanup of app")
        except LionException as e:
            LOGGER.error(f"App cleanup failed with exception '{e}'")

    def step(self, power: float, amb_temp: float):
        ffi_call(_lionl.lion_app_step(self._cdata, power, amb_temp), "Failed stepping")

    def run(self, power: Vectorizable, amb_temp: Vectorizable):
        try:
            power = Vector.new(power, dtypes.FLOAT64)
            amb_temp = Vector.new(amb_temp, dtypes.FLOAT64)
            ffi_call(
                _lionl.lion_app_run(self._cdata, power._cdata, amb_temp._cdata),
                "Failed running",
            )
        except TypeError:
            LOGGER.error("Trying to create vector from invalid type")
            raise TypeError(
                f"Could not create `Vector` from type '{
                    type(power).__name__}'"
            )
