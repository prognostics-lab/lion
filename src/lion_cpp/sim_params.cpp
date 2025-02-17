#include <lion/sim.h>
#include <lionpp/sim.hpp>

namespace lion {

SimParams::SimParams() : handle(lion_params_default()) {}

SimParams::~SimParams() {}

lion_params_init_t &SimParams::init() { return handle.init; }
lion_params_ehc_t  &SimParams::ehc() { return handle.ehc; }
lion_params_ocv_t  &SimParams::ocv() { return handle.ocv; }
lion_params_vft_t  &SimParams::vft() { return handle.vft; }
lion_params_temp_t &SimParams::temp() { return handle.temp; }
lion_params_rint_t &SimParams::rint() { return handle.rint; }

SimParams::operator lion_params_t() { return handle; }

lion_params_t *SimParams::get_handle() { return &handle; }

} // namespace lion
