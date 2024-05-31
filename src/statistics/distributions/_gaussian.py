import numpy as np


def pdf_gaussian(x: float, mu: float, sigma: float) -> float:
    return (1 / np.sqrt(2 * np.pi * sigma**2)) * np.exp(-(x - mu)**2 / (2 * sigma**2))


def cdf_gaussian(x: float, mu: float, sigma: float) -> float:
    raise NotImplementedError("CDF for gaussian not implemented")
