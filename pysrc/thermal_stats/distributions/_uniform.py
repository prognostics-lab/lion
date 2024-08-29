
import numpy as np


def pdf_uniform(x: float, a: float, b: float) -> float:
    return ((a <= x) & (x <= b)) / (b - a)


def cdf_uniform(x: float, a: float, b: float) -> float:
    raise NotImplementedError("CDF not implemented")
