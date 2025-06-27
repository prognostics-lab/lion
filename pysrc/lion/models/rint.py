from enum import Enum

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
        self.internal_resistance = internal_resistance or default.internal_resistance

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
        "poly",
    )

    def __init__(
        self,
        c40=None,
        c20=None,
        c10=None,
        c4=None,
        d5=None,
        d10=None,
        d15=None,
        d30=None,
        poly=None,
    ):
        # TODO: Implement configuring the polynomial coefficients
        default = self._c_default()
        self.c40 = c40 or default.c40
        self.c20 = c20 or default.c20
        self.c10 = c10 or default.c10
        self.c4 = c4 or default.c4
        self.d5 = d5 or default.d5
        self.d10 = d10 or default.d10
        self.d15 = d15 or default.d15
        self.d30 = d30 or default.d30
        self.poly = poly or default.poly

    def set_parameters(self, target):
        target.model = _ResistanceModels.POLARIZATION.value
        target.params.polarization.c40 = self.c40
        target.params.polarization.c20 = self.c20
        target.params.polarization.c10 = self.c10
        target.params.polarization.c4 = self.c4
        target.params.polarization.d5 = self.d5
        target.params.polarization.d10 = self.d10
        target.params.polarization.d15 = self.d15
        target.params.polarization.d30 = self.d30
        target.params.polarization.poly = self.poly

    @staticmethod
    def _c_default():
        return _lionl.lion_params_default_rint_polarization()
