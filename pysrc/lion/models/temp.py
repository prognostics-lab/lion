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
        self.cp = cp
        self.rin = rin
        self.rout = rout

    def set_parameters(self, target):
        target.cp = self.cp
        target.rin = self.rin
        target.rout = self.rout


class TemperatureAir(Temperature):
    def __init__(self):
        raise NotImplementedError("Air-aware temperature model not implemented")

    def set_parameters(self, target):
        raise NotImplementedError("Air-aware temperature model not implemented")
