#include "update.h"

#include <lion/lion.h>
#include <lion_math/lion_math.h>
#include <lion_utils/macros.h>

lion_status_t lion_slv_update(lion_sim_t *sim) {
  // This function assumes sim->state.{internal_temperature, soc_nominal, soh}
  // have been properly set, and spreads those initial values, and it also
  // assumes that sim->state.{power, ambient_temperature} have been filled with
  // the corresponding input
  sim->state.kappa            = lion_kappa(sim->state.internal_temperature, sim->params);
  sim->state.capacity_nominal = lion_capacity_nominal(sim->params.init.capacity, sim->state.soh);
  sim->state.soc_use          = lion_soc_usable(sim->state.soc_nominal, sim->state.kappa, sim->params);
  sim->state.capacity_use     = lion_capacity_usable(sim->state.capacity_nominal, sim->state.kappa, sim->params);
  sim->state.ehc              = lion_ehc(sim->state.soc_use, sim->params);

  sim->state.ref_open_circuit_voltage = lion_voc(sim->state.soc_use, sim->params);
  double voc_delta                    = sim->state.ehc * (sim->state.internal_temperature - sim->params->vft.tref);
  sim->state.open_circuit_voltage     = sim->state.ref_open_circuit_voltage + voc_delta;
  sim->state.current                  = lion_current_optimize(
      sim->sys_min,
      sim->state.power,
      sim->state.soc_use,
      sim->state.open_circuit_voltage,
      sim->state.current,
      sim->conf->sim_epsabs,
      sim->conf->sim_epsrel,
      sim->conf->sim_min_maxiter,
      sim->params
  );
  sim->state.internal_resistance = lion_resistance(sim->state.soc_use, sim->state.current, sim->state.soh, sim->params);
  sim->state.voltage             = lion_voltage_from_current(sim->state.power, sim->state.current, sim->params);

  sim->state.generated_heat =
      lion_generated_heat(sim->state.current, sim->state.internal_temperature, sim->state.internal_resistance, sim->state.ehc, sim->params);
  sim->state.surface_temperature = lion_surface_temperature(sim->state.internal_temperature, sim->state.ambient_temperature, sim->params);
  return LION_STATUS_SUCCESS;
}
