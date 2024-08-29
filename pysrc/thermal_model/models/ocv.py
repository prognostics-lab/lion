from collections import namedtuple

import numpy as np

from thermal_model.models.ehc import EHC_PARAMS, entropy_change, EHC
from thermal_model.models.thermal import THERMAL_PARAMS, SOC


OcvParams = namedtuple("OcvParams", "v0 vl alpha beta gamma")
OCV_V0 = 4.14
OCV_VL = 3.977
OCV_ALPHA = 0.15
OCV_BETA = 17
OCV_GAMMA = 10.5
OCV_PARAMS = OcvParams(OCV_V0, OCV_VL, OCV_ALPHA, OCV_BETA, OCV_GAMMA)


def calculate_ocv(soc, ocv_params=OCV_PARAMS):
    ocv = ocv_params.vl
    ocv += (ocv_params.v0 - ocv_params.vl) * \
        np.exp(ocv_params.gamma * (soc - 1))
    ocv += ocv_params.alpha * ocv_params.vl * (soc - 1)
    ocv += (1 - ocv_params.alpha) * ocv_params.vl * \
        (np.exp(-ocv_params.beta) - np.exp(-ocv_params.beta * np.sqrt(soc)))
    return ocv


def calculate_ocv_grad(soc, ocv_params=OCV_PARAMS):
    grad = ocv_params.gamma * \
        (ocv_params.v0 - ocv_params.vl) * np.exp(ocv_params.gamma * (soc - 1))
    grad += ocv_params.alpha * ocv_params.vl
    grad += ((1 - ocv_params.alpha) * ocv_params.vl * ocv_params.beta *
             np.exp(-ocv_params.beta * np.sqrt(soc))) / (2 * np.sqrt(soc))
    return grad


def calculate_ocv_temperature(soc, temperature, ocv_params=OCV_PARAMS, ehc_params=EHC_PARAMS, th_params=THERMAL_PARAMS):
    return calculate_ocv(soc, ocv_params) + (temperature - th_params.t_ref) * entropy_change(soc, ehc_params)


class BurgosOcv:
    """Class for calculation of OCV using Burgos' model"""

    def __init__(self, ocv_params=OCV_PARAMS, ehc_model=None, thermal_model=None):
        self.params = ocv_params
        self.ehc = ehc_model
        self.soc = thermal_model

    def evaluate(self, soc, temperature=None):
        if temperature is None or self.ehc is None or self.soc is None:
            return calculate_ocv(soc, self.params)
        else:
            return calculate_ocv_temperature(soc, temperature, self.params, self.ehc.params, self.soc.params)

    def grad(self, soc):
        return calculate_ocv_grad(soc, self.params)


BURGOS_OCV = BurgosOcv(OCV_PARAMS, EHC, SOC)
