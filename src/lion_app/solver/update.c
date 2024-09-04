#include <lion_math/lion_math.h>
#include <lion_utils/macros.h>

#include <lion/lion.h>
#include <lion/solver/update.h>

lion_status_t _spread_state(lion_app_t *app) {
  // This function assumes app->state->{internal_temperature, soc_nominal}
  // have been properly set, and spreads those initial values
  app->state->kappa = lion_kappa(app->state->internal_temperature, app->params);
  app->state->capacity_nominal = app->params->init.initial_capacity;
  app->state->soc_use =
      lion_soc_usable(app->state->soc_nominal, app->state->kappa, app->params);
  app->state->capacity_use = lion_capacity_usable(
      app->state->capacity_nominal, app->state->kappa, app->params);

  app->state->open_circuit_voltage =
      lion_voc(app->state->soc_nominal, app->params);
  // TODO: Add current calculation
  return LION_STATUS_SUCCESS;
}

lion_status_t lion_slv_update(lion_app_t *app, double power, double amb_temp) {
  // Assumes the states have already been set
  app->state->power = power;
  app->state->ambient_temperature = amb_temp;
  LION_CALL_I(_spread_state(app), "Failed to calculate rest of the parameters");
  return LION_STATUS_SUCCESS;
}
