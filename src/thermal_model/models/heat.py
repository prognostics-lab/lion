from thermal_model.models.ehc import entropy_change


def generated_heat_from_current(current, temperature, soc, internal_resistance):
    return current ** 2 * internal_resistance - current * temperature * entropy_change(soc)
