#include <lion/lion.h>
#include <lion/solver/update.h>
#include <lion_math/lion_math.h>
#include <lion_utils/macros.h>

lion_status_t lion_slv_update(lion_app_t *app) {
  // This function assumes app->state.{internal_temperature, soc_nominal}
  // have been properly set, and spreads those initial values, and it also
  // assumes that app->state.{power, ambient_temperature} have been filled with
  // the corresponding input
  app->state.kappa            = lion_kappa(app->state.internal_temperature, app->params);
  app->state.capacity_nominal = app->params->init.capacity;
  app->state.soc_use          = lion_soc_usable(app->state.soc_nominal, app->state.kappa, app->params);
  app->state.capacity_use     = lion_capacity_usable(app->state.capacity_nominal, app->state.kappa, app->params);

  app->state.open_circuit_voltage = lion_voc(app->state.soc_use, app->params);
<<<<<<< HEAD
  app->state.current =
      lion_current_optimize(app->sys_min, app->state.power, app->state.soc_use,
                            app->state.open_circuit_voltage, app->state.current,
                            app->conf->sim_epsabs, app->conf->sim_epsrel,
                            app->conf->sim_min_maxiter, app->params);
  app->state.internal_resistance =
      lion_resistance(app->state.soc_use, app->state.current, app->params);
  app->state.voltage = lion_voltage_from_current(
      app->state.power, app->state.current, app->params);
=======
  app->state.current              = lion_current_optimize(
      app->sys_min,
      app->state.power,
      app->state.soc_use,
      app->state.open_circuit_voltage,
      app->state.current,
      app->conf->sim_epsabs,
      app->conf->sim_epsrel,
      app->conf->sim_min_maxiter,
      app->params
  );
  app->state.internal_resistance = lion_resistance(app->state.soc_use, app->state.current, app->params);
  app->state.voltage             = lion_voltage_from_current(app->state.power, app->state.current, app->params);
>>>>>>> 4db05ab (I think only format changes)

  app->state.ehc = lion_ehc(app->state.soc_use, app->params);
  app->state.generated_heat =
      lion_generated_heat(app->state.current, app->state.internal_temperature, app->state.internal_resistance, app->state.ehc, app->params);
  app->state.surface_temperature = lion_surface_temperature(app->state.internal_temperature, app->state.ambient_temperature, app->params);
  return LION_STATUS_SUCCESS;
}
