from enum import Enum

from lion_utils.choose import choose

import lion_ffi as _
from lion._lion import ffi
from lion._lion import lib as _lionl
from lion.models._base import _BaseParams


class _ResistanceModels(Enum):
    FIXED = _lionl.LION_RINT_MODEL_FIXED
    POLARIZATION = _lionl.LION_RINT_MODEL_POLARIZATION


# TODO: Test whether this implementation fails when setting different models


class Resistance(_BaseParams):
    """Base class for resistance models"""


class ResistanceFixed(Resistance):
    __slots__ = ("internal_resistance",)

    def __init__(self, internal_resistance=None):
        default = self._c_default()
        self.internal_resistance = choose(internal_resistance, default.internal_resistance)

    def set_parameters(self, target):
        target.model = _ResistanceModels.FIXED.value
        target.params.fixed.internal_resistance = self.internal_resistance

    @staticmethod
    def _c_default():
        return _lionl.lion_params_default_rint_fixed()


class ResistancePolarization(Resistance):
    __slots__ = (
        "c40",
        "c20",
        "c10",
        "c4",
        "d5",
        "d10",
        "d15",
        "d30",
    )

    def __init__(
        self,
        c40,
        c20,
        c10,
        c4,
        d5,
        d10,
        d15,
        d30,
    ):
        default = self._c_default()
        self.c40 = choose(c40, default.c40)
        self.c20 = choose(c20, default.c20)
        self.c10 = choose(c10, default.c10)
        self.c4  = choose(c4, default.c4)
        self.d5  = choose(d5, default.d5)
        self.d10 = choose(d10, default.d10)
        self.d15 = choose(d15, default.d15)
        self.d30 = choose(d30, default.d30)

    def set_parameters(self, target):
        # TODO: Implement configuring the polynomial coefficients
        target.model = _ResistanceModels.POLARIZATION.value
        target.params.polarization.c40 = self.c40
        target.params.polarization.c20 = self.c20
        target.params.polarization.c10 = self.c10
        target.params.polarization.c4 = self.c4
        target.params.polarization.d5 = self.d5
        target.params.polarization.d10 = self.d10
        target.params.polarization.d15 = self.d15
        target.params.polarization.d30 = self.d30

    @staticmethod
    def _c_default():
        return _lionl.lion_params_default_rint_polarization()
