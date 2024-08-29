from thermal_model.models import BURGOS_OCV, SOC, EHC, generated_heat_from_current, OcvParams, ThermalParams, EhcParams, BurgosOcv, StateOfCharge, EntropicHeatCoefficient
from thermal_model.logger import LOGGER


class Battery:
    def __init__(self, ocv=BURGOS_OCV, thermal=SOC, ehc=EHC) -> None:
        self.ocv = ocv
        self.thermal = thermal
        self.ehc = ehc

    @classmethod
    def from_params(cls, **params):
        ocv_params = {}
        th_params = {}
        ehc_params = {}
        for key, val in params:
            if key in OcvParams._fields:
                ocv_params[key] = val
            elif key in ThermalParams._fields:
                th_params[key] = val
            elif key in EhcParams._fields:
                ehc_params[key] = val
            else:
                LOGGER.warning(f"Found invalid key '{key}'")
        thermal = StateOfCharge(ThermalParams(**th_params))
        ehc = EntropicHeatCoefficient(EhcParams(**ehc_params))
        ocv = BurgosOcv(OcvParams(**ocv_params), ehc, thermal)
        return cls(ocv, thermal, ehc)


BATTERY = Battery(BURGOS_OCV, SOC, EHC)
