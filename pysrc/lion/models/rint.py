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
        self.internal_resistance = internal_resistance

    def set_parameters(self, target):
        target.model = _ResistanceModels.FIXED
        target.params.fixed.internal_resistance = self.internal_resistance


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
        self.c40 = c40
        self.c20 = c20
        self.c10 = c10
        self.c4 = c4
        self.d5 = d5
        self.d10 = d10
        self.d15 = d15
        self.d30 = d30

    def set_parameters(self, target):
        # TODO: Implement configuring the polynomial coefficients
        target.model = _ResistanceModels.POLARIZATION
        target.params.polarization.c40 = self.c40
        target.params.polarization.c20 = self.c20
        target.params.polarization.c10 = self.c10
        target.params.polarization.c4 = self.c4
        target.params.polarization.d5 = self.d5
        target.params.polarization.d10 = self.d10
        target.params.polarization.d15 = self.d15
        target.params.polarization.d30 = self.d30
