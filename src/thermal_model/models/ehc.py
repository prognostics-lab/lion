from collections import namedtuple

import numpy as np


EhcParams = namedtuple("EhcParams", "amplitude kappa mu sigma l")
EHC_AMPLITUDE = 0.04e-3
EHC_KAPPA = 3
EHC_MU = 0.4
EHC_SIGMA = 0.05
EHC_LAMBDA = 7
EHC_PARAMS = EhcParams(EHC_AMPLITUDE, EHC_KAPPA, EHC_MU, EHC_SIGMA, EHC_LAMBDA)


def entropy_change(soc, ehc_params=EHC_PARAMS):
    ehc = np.exp(-(soc - ehc_params.mu)**2 / (2 * ehc_params.sigma)) / np.sqrt(2 * np.pi * ehc_params.sigma ** 2)
    ehc -= ehc_params.l * np.exp(-ehc_params.kappa * soc)
    ehc *= ehc_params.amplitude
    return ehc


class EntropicHeatCoefficient:
    def __init__(self, ehc_params=EHC_PARAMS):
        self.ehc_params = ehc_params

    def evaluate(self, soc):
        return entropy_change(soc, self.ehc_params)


EHC = EntropicHeatCoefficient(EHC_PARAMS)
