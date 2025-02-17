#include <lion/sim.h>
#include <lionpp/sim.hpp>

namespace lion {

SimConfig::SimConfig() : handle(lion_sim_config_default()) {}

SimConfig::~SimConfig() {}

SimConfig::operator lion_sim_config() { return handle; }

lion_sim_config_t *SimConfig::get_handle() { return &handle; }

} // namespace lion
