#include <lion/app.h>
#include <lionpp/app.hpp>

namespace lion {

AppConfig::AppConfig() : handle(lion_app_config_default()) {}

AppConfig::~AppConfig() {}

AppConfig::operator lion_app_config() { return handle; }

lion_app_config_t *AppConfig::get_handle() { return &handle; }

} // namespace lion
