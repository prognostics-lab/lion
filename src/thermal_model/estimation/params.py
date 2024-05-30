from typing import Dict
from collections import namedtuple

import numpy as np


_NAMES = ["cp", "cair", "rair", "rin", "rout"]

TargetParams = namedtuple("TargetParams", " ".join(_NAMES))
TargetNLParams = namedtuple("TargetNLParams", "cp cair rair rin rout k")


def params_prefilled(p: np.ndarray, vals: Dict[str, float]) -> np.ndarray:
    final_params = np.zeros(len(_NAMES))
    i = 0
    for j, name in enumerate(_NAMES):
        if name in vals:
            final_params[j] = vals[name]
        else:
            final_params[j] = p[i]
            i += 1
    return final_params
