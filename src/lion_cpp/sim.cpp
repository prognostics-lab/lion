#include <lion/sim.h>
#include <lion/vector.h>
#include <lionpp/sim.hpp>
#include <stdexcept>

namespace lion {

Sim::Sim(SimConfig *conf, SimParams *params) {
  handle            = new lion_sim_t;
  lion_status_t ret = lion_sim_new(conf->get_handle(), params->get_handle(), handle);
  if (ret != LION_STATUS_SUCCESS) {
    throw std::runtime_error("Failed to create sim");
  }
}

Sim::~Sim() {
  lion_sim_cleanup(handle);
  delete handle;
}

Sim::operator lion_sim_t *() { return handle; }

Status Sim::step(double power, double amb_temp) { return static_cast<Status>(lion_sim_step(handle, power, amb_temp)); }

Status Sim::run(std::vector<double> const &power, std::vector<double> const &amb_temp) {
  lion_vector_t power_vec;
  lion_vector_t amb_vec;
  lion_status_t ret = lion_vector_from_array(handle, power.data(), power.size(), sizeof(double), &power_vec);
  if (ret != LION_STATUS_SUCCESS) {
    return Status::FAILURE;
  }
  ret = lion_vector_from_array(handle, amb_temp.data(), amb_temp.size(), sizeof(double), &amb_vec);
  if (ret != LION_STATUS_SUCCESS) {
    return Status::FAILURE;
  }

  Status out = static_cast<Status>(lion_sim_run(handle, &power_vec, &amb_vec));
  lion_vector_cleanup(handle, &power_vec);
  lion_vector_cleanup(handle, &amb_vec);
  return out;
}

bool Sim::should_close() const { return lion_sim_should_close(handle); }

uint64_t Sim::max_iters() const { return lion_sim_max_iters(handle); }

} // namespace lion
