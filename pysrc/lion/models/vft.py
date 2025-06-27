from lion_utils.choose import choose

import lion_ffi as _
from lion._lion import ffi
from lion._lion import lib as _lionl
from lion.models._base import _BaseParams


class Vft(_BaseParams):
    __slots__ = (
        "k1",
        "k2",
        "tref",
    )

    def __init__(self, k1=None, k2=None, tref=None):
        default = self._c_default()
        self.k1 = k1 or default.k1
        self.k2 = k2 or default.k2
        self.tref = tref or default.tref

    def set_parameters(self, target):
        target.k1 = self.k1
        target.k2 = self.k2
        target.tref = self.tref

    @staticmethod
    def _c_default():
        return _lionl.lion_params_default_vft()
