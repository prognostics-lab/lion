from collections import namedtuple
from typing import Callable

import lion_ffi as _
from _lion import ffi
from _lion import lib as _lionl
from lion import dtypes
from lion.exceptions import LionException
from lion.status import Status, ffi_call
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


class Config:
    """Lion application configuration"""

    __slots__ = (
        "_cdata",
        "init",
        "update",
        "finished",
    )

    def __init__(self, name=None, init=None, update=None, finished=None):
        self._cdata = _lionl.lion_app_config_default()
        if name is not None:
            self._cdata.app_name = name

        self.init = init
        self.update = update
        self.finished = finished

    @property
    def name(self):
        return self._cdata.app_name

    @name.setter
    def name(self, new_name: str):
        self._cdata.app_name = new_name

    def _generate_callbacks(self):
        if self.init is not None:
            self._cdata.init_hook = _generate_init_pythoncb(self.init)
        if self.update is not None:
            self._cdata.update_hook = _generate_update_pythoncb(self.update)
        if self.finished is not None:
            self._cdata.finished_hook = _generate_finished_pythoncb(self.finished)


ParamsInit = namedtuple("ParamsInit", "soc, soh, temp_in, capacity, current_guess")
ParamsEHC = namedtuple("ParamsEHC", "a, b, kappa, mu, sigma, lambda")
ParamsOCV = namedtuple("ParamsOCV", "alpha, beta, gamma, v0, vl")
ParamsVFT = namedtuple("ParamsVFT", "k1, k2, tref")
ParamsTemp = namedtuple("ParamsTemp", "cp, rin, rout")


class Params:
    """Lion application parameters"""

    __slots__ = ("_cdata",)

    def __init__(self):
        self._cdata = _lionl.lion_params_default()

    # TODO: Figure out how to handle parameters


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
