#include <lion/app.h>
#include <lionpp/app.hpp>

namespace lion {

AppParams::AppParams() : handle(lion_params_default()) {}

AppParams::~AppParams() {}

lion_params_init_t &AppParams::init() { return handle.init; }
lion_params_ehc_t &AppParams::ehc() { return handle.ehc; }
lion_params_ocv_t &AppParams::ocv() { return handle.ocv; }
lion_params_vft_t &AppParams::vft() { return handle.vft; }
lion_params_temp_t &AppParams::temp() { return handle.t; }
lion_params_rint_t &AppParams::rint() { return handle.rint; }

AppParams::operator lion_params_t() { return handle; }

lion_params_t *AppParams::get_handle() { return &handle; }

} // namespace lion
