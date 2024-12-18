from lion.models._base import _BaseParams


class Initial(_BaseParams):
    __slots__ = (
        "soc",
        "temp_in",
        "soh",
        "capacity",
        "current_guess",
    )

    def __init__(
        self, soc=None, temp_in=None, soh=None, capacity=None, current_guess=None
    ):
        self.soc = soc
        self.temp_in = temp_in
        self.soh = soh
        self.capacity = capacity
        self.current_guess = current_guess

    def set_parameters(self, target):
        target.soc = self.soc
        target.temp_in = self.temp_in
        target.soh = self.soh
        target.capacity = self.capacity
        target.current_guess = self.current_guess
