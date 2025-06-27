from lion_utils.choose import choose

import lion_ffi as _
from lion._lion import ffi
from lion._lion import lib as _lionl
from lion.models._base import _BaseParams


class Ocv(_BaseParams):
    __slots__ = (
        "alpha",
        "beta",
        "gamma",
        "v0",
        "vl",
    )

    def __init__(self, alpha=None, beta=None, gamma=None, v0=None, vl=None):
        default = self._c_default()
        self.alpha = alpha or default.alpha
        self.beta = beta or default.beta
        self.gamma = gamma or default.gamma
        self.v0 = v0 or default.v0
        self.vl = vl or default.vl

    def set_parameters(self, target):
        target.alpha = self.alpha
        target.beta = self.beta
        target.gamma = self.gamma
        target.v0 = self.v0
        target.vl = self.vl

    @staticmethod
    def _c_default():
        return _lionl.lion_params_default_ocv()
