from lion.models._base import _BaseParams


class Vft(_BaseParams):
    __slots__ = (
        "k1",
        "k2",
        "tref",
    )

    def __init__(self, k1=None, k2=None, tref=None):
        self.k1 = k1
        self.k2 = k2
        self.tref = tref

    def set_parameters(self, target):
        target.k1 = self.k1
        target.k2 = self.k2
        target.tref = self.tref
