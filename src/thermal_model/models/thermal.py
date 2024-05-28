from collections import namedtuple

import numpy as np


ThermalParams = namedtuple("ThermalParams", "a b t_ref")
THERMAL_T_REF = 2.980e2
THERMAL_ALPHA = -5.738
THERMAL_BETA = 2.099e2
THERMAL_PARAMS = ThermalParams(THERMAL_ALPHA, THERMAL_BETA, THERMAL_T_REF)


def calculate_soc_coeff(temperature, params=THERMAL_PARAMS):
    return np.exp(params.a / (temperature - params.b) - params.a / (params.t_ref - params.b))


def calculate_soc_with_temperature(soc, temperature, params=THERMAL_PARAMS):
    return 1 + (soc - 1) / calculate_soc_coeff(temperature, params)


# def calculate_entropy_change(temperature, soc, th_params=THERMAL_PARAMS, ocv_params=OCV_PARAMS):
#     return (th_params.a / ((temperature - th_params.b)**2)) * soc * calculate_ocv_grad(soc, ocv_params)


# def calculate_entropy_change_kappa(temperature, soc, t_ref=T_REF, a=ALPHA, b=BETA, ocv_params=OCV_PARAMS):
#     return (a / ((temperature - b)**2 * calculate_soc_coeff(temperature, t_ref, a, b))) * soc * calculate_ocv_grad(soc, ocv_params)


class StateOfCharge:
    def __init__(self, th_params=THERMAL_PARAMS):
        self.params = th_params

    def evaluate(self, soc, temperature):
        return calculate_soc_with_temperature(soc, temperature, self.params)


SOC = StateOfCharge(THERMAL_PARAMS)
