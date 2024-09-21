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


def _generate_init_pythoncb(app: "App", func: Callable[["App"], Status]):
    @ffi.def_extern()
    def init_pythoncb(_):
        return func(app).value

    return init_pythoncb


def _generate_update_pythoncb(app: "App", func: Callable[["App"], Status]):
    @ffi.def_extern()
    def update_pythoncb(_):
        return func(app).value

    return update_pythoncb


def _generate_finished_pythoncb(app: "App", func: Callable[["App"], Status]):
    @ffi.def_extern()
    def finished_pythoncb(_):
        return func(app).value

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

    # TODO: Figure out how to do parameters


class State:
    __slots__ = ("_app",)

    def __init__(self, app: "App"):
        self._app = app

    def as_dict(self) -> dict:
        return {
            "time": self.time,
            "step": self.step,
            "power": self.power,
            "ambient_temperature": self.ambient_temperature,
            "voltage": self.voltage,
            "current": self.current,
            "open_circuit_voltage": self.open_circuit_voltage,
            "internal_resistance": self.internal_resistance,
            "ehc": self.ehc,
            "generated_heat": self.generated_heat,
            "internal_temperature": self.internal_temperature,
            "surface_temperature": self.surface_temperature,
            "kappa": self.kappa,
            "soc_nominal": self.soc_nominal,
            "capacity_nominal": self.capacity_nominal,
            "soc_use": self.soc_use,
            "capacity_use": self.capacity_use,
        }

    def as_table(self) -> str:
        return f"""\
*** App state ***
-> time: {self.time}
-> step: {self.step}
-> power: {self.power}
-> ambient_temperature: {self.ambient_temperature}
-> voltage: {self.voltage}
-> current: {self.current}
-> open_circuit_voltage: {self.open_circuit_voltage}
-> internal_resistance: {self.internal_resistance}
-> ehc: {self.ehc}
-> generated_heat: {self.generated_heat}
-> internal_temperature: {self.internal_temperature}
-> surface_temperature: {self.surface_temperature}
-> kappa: {self.kappa}
-> soc_nominal: {self.soc_nominal}
-> capacity_nominal: {self.capacity_nominal}
-> soc_use: {self.soc_use}
-> capacity_use: {self.capacity_use}
"""

    @property
    def time(self) -> float:
        return self._app._cdata.state.time

    @property
    def step(self) -> int:
        return self._app._cdata.state.step

    @property
    def power(self) -> float:
        return self._app._cdata.state.power

    @property
    def ambient_temperature(self) -> float:
        return self._app._cdata.state.ambient_temperature

    @property
    def voltage(self) -> float:
        return self._app._cdata.state.voltage

    @property
    def current(self) -> float:
        return self._app._cdata.state.current

    @property
    def open_circuit_voltage(self) -> float:
        return self._app._cdata.state.open_circuit_voltage

    @property
    def internal_resistance(self) -> float:
        return self._app._cdata.state.internal_resistance

    @property
    def ehc(self) -> float:
        return self._app._cdata.state.ehc

    @property
    def generated_heat(self) -> float:
        return self._app._cdata.state.generated_heat

    @property
    def internal_temperature(self) -> float:
        return self._app._cdata.state.internal_temperature

    @property
    def surface_temperature(self) -> float:
        return self._app._cdata.state.surface_temperature

    @property
    def kappa(self) -> float:
        return self._app._cdata.state.kappa

    @property
    def soc_nominal(self) -> float:
        return self._app._cdata.state.soc_nominal

    @property
    def capacity_nominal(self) -> float:
        return self._app._cdata.state.capacity_nominal

    @property
    def soc_use(self) -> float:
        return self._app._cdata.state.soc_use

    @property
    def capacity_use(self) -> float:
        return self._app._cdata.state.capacity_use


class App:
    """Lion application to run"""

    __slots__ = ("_cdata", "_initialized", "state", "config", "params")

    def __init__(
        self,
        config: Config | None = None,
        params: Params | None = None,
        init: Callable[["App"], State] | None = None,
        update: Callable[["App"], State] | None = None,
        finished: Callable[["App"], State] | None = None,
    ):
        self._cdata = ffi.new("lion_app_t *")
        self._initialized = False
        if config is None:
            self.config = Config()
        else:
            self.config = config
        if params is None:
            self.params = Params()
        else:
            self.params = params
        _lionl.lion_app_new(self.config._cdata, self.params._cdata, self._cdata)

        self.state = State(self)

        if init is not None:
            self.init_hook = init
        if update is not None:
            self.update_hook = update
        if finished is not None:
            self.finished_hook = finished

    def __del__(self):
        try:
            ffi_call(_lionl.lion_app_cleanup(self._cdata), "Failed cleanup of app")
        except LionException as e:
            LOGGER.error(f"App cleanup failed with exception '{e}'")

    def init(self, initial_power: float, initial_amb_temp: float):
        ffi_call(
            _lionl.lion_app_init(self._cdata, initial_power, initial_amb_temp),
            "Failed initializing",
        )
        self._initialized = True

    def step(self, power: float, amb_temp: float):
        if not self._initialized:
            LOGGER.warn("Auto-initializing before step")
            self.init(power, amb_temp)
        else:
            ffi_call(
                _lionl.lion_app_step(self._cdata, power, amb_temp), "Failed stepping"
            )

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

    @property
    def init_hook(self) -> None:
        raise NotImplementedError("Can't fetch C functions")

    @init_hook.setter
    def init_hook(self, new_func: Callable[["App"], Status]):
        _generate_init_pythoncb(self, new_func)
        self._cdata.init_hook = _lionl.init_pythoncb

    @property
    def update_hook(self) -> None:
        raise NotImplementedError("Can't fetch C functions")

    @update_hook.setter
    def update_hook(self, new_func: Callable[["App"], Status]):
        _generate_update_pythoncb(self, new_func)
        self._cdata.update_hook = _lionl.update_pythoncb

    @property
    def finished_hook(self) -> None:
        raise NotImplementedError("Can't fetch C functions")

    @finished_hook.setter
    def finished_hook(self, new_func: Callable[["App"], Status]):
        _generate_finished_pythoncb(self, new_func)
        self._cdata.finished_hook = _lionl.finished_pythoncb
