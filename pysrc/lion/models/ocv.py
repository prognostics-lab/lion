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
        self.alpha = alpha
        self.beta = beta
        self.gamma = gamma
        self.v0 = v0
        self.vl = vl

    def set_parameters(self, target):
        target.alpha = self.alpha
        target.beta = self.beta
        target.gamma = self.gamma
        target.v0 = self.v0
        target.vl = self.vl
