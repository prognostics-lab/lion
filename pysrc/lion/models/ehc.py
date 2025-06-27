from lion_utils.choose import choose

import lion_ffi as _
from lion._lion import ffi
from lion._lion import lib as _lionl
from lion.models._base import _BaseParams


class Ehc(_BaseParams):
    __slots__ = (
        "a",
        "b",
        "mu",
        "kappa",
        "sigma",
        "l",
    )

    def __init__(self, a=None, b=None, mu=None, kappa=None, sigma=None, l=None):
        default = self._c_default()
        self.a = a or default.a
        self.b = b or default.b
        self.mu = mu or default.mu
        self.kappa = kappa or default.kappa
        self.sigma = sigma or default.sigma
        self.l = l or default.l

    def set_parameters(self, target):
        target.a = self.a
        target.b = self.b
        target.mu = self.mu
        target.kappa = self.kappa
        target.sigma = self.sigma
        target.l = self.l

    @staticmethod
    def _c_default():
        return _lionl.lion_params_default_ehc()
