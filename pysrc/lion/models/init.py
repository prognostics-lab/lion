from lion_utils.choose import choose

import lion_ffi as _
from lion._lion import ffi
from lion._lion import lib as _lionl
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
        default = self._c_default()
        self.soc = choose(soc, default.soc)
        self.temp_in = choose(temp_in, default.temp_in)
        self.soh = choose(soh, default.soh)
        self.capacity = choose(capacity, default.capacity)
        self.current_guess = choose(current_guess, default.current_guess)

    def set_parameters(self, target):
        target.soc = self.soc
        target.temp_in = self.temp_in
        target.soh = self.soh
        target.capacity = self.capacity
        target.current_guess = self.current_guess

    @staticmethod
    def _c_default():
        return _lionl.lion_params_default_init()
