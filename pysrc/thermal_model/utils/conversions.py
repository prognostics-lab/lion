ZERO_CELSIUS_IN_KELVIN = 273


def celsius_to_kelvin(deg):
    return ZERO_CELSIUS_IN_KELVIN + deg


def kelvin_to_celsius(deg):
    return deg - ZERO_CELSIUS_IN_KELVIN
