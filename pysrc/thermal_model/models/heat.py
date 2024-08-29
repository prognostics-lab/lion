import numpy as np

from thermal_model.models.ehc import entropy_change


def generated_heat_from_current(current, temperature, soc, internal_resistance):
    return np.clip(current ** 2 * internal_resistance - current * temperature * entropy_change(soc), 0, np.inf)
