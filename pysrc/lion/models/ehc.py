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
        self.a = a
        self.b = b
        self.mu = mu
        self.kappa = kappa
        self.sigma = sigma
        self.l = l

    def set_parameters(self, target):
        target.a = self.a
        target.b = self.b
        target.mu = self.mu
        target.kappa = self.kappa
        target.sigma = self.sigma
        target.l = self.l
