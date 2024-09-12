from enum import Enum

import lion_ffi as _
from _lion import ffi
from _lion import lib as _lionl
from lion import dtypes
from lion.exceptions import LionException
from lion.status import ffi_call
from lion_utils.logger import LOGGER


class Regime(Enum):
    ONLYSF = _lionl.LION_APP_ONLYSF
    ONLYAIR = _lionl.LION_APP_ONLYAIR
    BOTH = _lionl.LION_APP_BOTH


class Stepper(Enum):
    RK2 = _lionl.LION_STEPPER_RK2
    RK4 = _lionl.LION_STEPPER_RK4
    RKF45 = _lionl.LION_STEPPER_RKF45
    RKCK = _lionl.LION_STEPPER_RKCK
    RK8PD = _lionl.LION_STEPPER_RK8PD
    RK1IMP = _lionl.LION_STEPPER_RK1IMP
    RK2IMP = _lionl.LION_STEPPER_RK2IMP
    RK4IMP = _lionl.LION_STEPPER_RK4IMP
    BSIMP = _lionl.LION_STEPPER_BSIMP
    MSADAMS = _lionl.LION_STEPPER_MSADAMS
    MSBDF = _lionl.LION_STEPPER_MSBDF


class Minimizer(Enum):
    GOLDENSECTION = _lionl.LION_MINIMIZER_GOLDENSECTION
    BRENT = _lionl.LION_MINIMIZER_BRENT
    QUADGOLDEN = _lionl.LION_MINIMIZER_QUADGOLDEN
