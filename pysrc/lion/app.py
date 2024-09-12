from typing import Callable

import lion_ffi as _
from _lion import ffi
from _lion import lib as _lionl
from lion import dtypes
from lion.exceptions import LionException
from lion.status import ffi_call
from lion_utils.logger import LOGGER


class AppConfig:
    """Lion application configuration"""

    __slots__ = ("_cdata",)

    def __init__(self, name=None):
        self._cdata = _lionl.lion_app_config_default()
        if name is not None:
            self._cdata.app_name = name

    @property
    def name(self):
        return self._cdata.app_name

    @name.setter
    def name(self, new_name: str):
        self._cdata.app_name = new_name


class App:
    """Lion application to run"""

    __slots__ = ("_cdata",)

    def __init__(self):
        pass
