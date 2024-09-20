#include <stdexcept>

#include <lion/app.h>
#include <lion/vector.h>
#include <lionpp/app.hpp>

namespace lion {

App::App(AppConfig *conf, AppParams *params) {
  handle = new lion_app_t;
  lion_status_t ret =
      lion_app_new(conf->get_handle(), params->get_handle(), handle);
  if (ret != LION_STATUS_SUCCESS) {
    throw std::runtime_error("Failed to create app");
  }
}

App::~App() {
  lion_app_cleanup(handle);
  delete handle;
}

App::operator lion_app_t *() { return handle; }

Status App::step(double power, double amb_temp) {
  return static_cast<Status>(lion_app_step(handle, power, amb_temp));
}

Status App::run(std::vector<double> const &power,
                std::vector<double> const &amb_temp) {
  lion_vector_t power_vec;
  lion_vector_t amb_vec;
  lion_status_t ret = lion_vector_from_array(handle, power.data(), power.size(),
                                             sizeof(double), &power_vec);
  if (ret != LION_STATUS_SUCCESS) {
    return Status::FAILURE;
  }
  ret = lion_vector_from_array(handle, amb_temp.data(), amb_temp.size(),
                               sizeof(double), &amb_vec);
  if (ret != LION_STATUS_SUCCESS) {
    return Status::FAILURE;
  }

  Status out = static_cast<Status>(lion_app_run(handle, &power_vec, &amb_vec));
  lion_vector_cleanup(handle, &power_vec);
  lion_vector_cleanup(handle, &amb_vec);
  return out;
}

bool App::should_close() const { return lion_app_should_close(handle); }

uint64_t App::max_iters() const { return lion_app_max_iters(handle); }

} // namespace lion
