from lion_utils.choose import choose

import lion_ffi as _
from lion._lion import ffi
from lion._lion import lib as _lionl
from lion.models._base import _BaseParams


class Temperature(_BaseParams):
    """Base class for temperature models"""


class TemperatureSurface(Temperature):
    __slots__ = (
        "cp",
        "rin",
        "rout",
    )

    def __init__(self, cp=None, rin=None, rout=None):
        default = self._c_default()
        self.cp = cp or default.cp
        self.rin = rin or default.rin
        self.rout = rout or default.rout

    def set_parameters(self, target):
        target.cp = self.cp
        target.rin = self.rin
        target.rout = self.rout

    @staticmethod
    def _c_default():
        # TODO: Change this when I eventually decide to implement other models
        return _lionl.lion_params_default_temp()


class TemperatureAir(Temperature):
    def __init__(self):
        raise NotImplementedError("Air-aware temperature model not implemented")

    def set_parameters(self, target):
        raise NotImplementedError("Air-aware temperature model not implemented")
