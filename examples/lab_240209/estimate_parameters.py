import os
import sys
import pathlib

import pandas as pd

src_path = pathlib.Path.joinpath(pathlib.Path(os.getcwd()), "src")
datalib_path = pathlib.Path.joinpath(pathlib.Path(os.getcwd()), "data")
sys.path.append(str(src_path))
sys.path.append(str(datalib_path))
# pylint: disable=import-error
from thermal_model.estimation import lti_from_data
from thermal_model.estimation.models import generate_evaluation
from thermal_model.models import (
    generated_heat_from_current,
    EhcParams,
    OcvParams,
    ThermalParams,
    calculate_soc_coeff,
    calculate_soc_with_temperature,
    calculate_ocv_temperature,
    calculate_ocv,
    calculate_ocv,
)

from lib_240209_temptest_C6B2 import get_data
# pylint: enable=import-error


def main():
    pass

