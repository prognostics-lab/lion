from lion_utils.choose import choose

import lion_ffi as _
from lion._lion import ffi
from lion._lion import lib as _lionl
from lion.models._base import _BaseParams


class Soh(_BaseParams):
    __slots__ = (
        "total_cycles",
        "final_soh",
    )

    def __init__(self, total_cycles=None, final_soh=None):
        default = self._c_default()
        self.total_cycles = choose(total_cycles, default.total_cycles)
        self.final_soh = choose(final_soh, default.final_soh)

    def set_parameters(self, target):
        target.total_cycles = self.total_cycles
        target.final_soh = self.final_soh

    @staticmethod
    def _c_default():
        return _lionl.lion_params_default_soh()
