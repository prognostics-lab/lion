from enum import Enum

import lion_ffi as _
from _lion import ffi
from _lion import lib as _lionl
from lion.exceptions import LionException
from lion_utils.logger import LOGGER


class Status(Enum):
    SUCCESS = _lionl.LION_STATUS_SUCCESS
    FAILURE = _lionl.LION_STATUS_FAILURE
    EXIT = _lionl.LION_STATUS_EXIT


def ffi_call(val, msg: str | None = None, *, exception_cls=LionException):
    if val == Status.FAILURE.value:
        LOGGER.error(f"Found a failed lion call %s", "" if msg is None else f"'{msg}'")
        if msg is None:
            raise exception_cls("lion call failed")
        else:
            raise exception_cls(msg)
