#include "Plugin.h"
#include "Global.h"

namespace plugin {

Plugin::Plugin(ll::plugin::NativePlugin& self) : mSelf(self) {
    // Code for loading the plugin goes here.
    GMLIB::loadLib();
}

bool Plugin::enable() {
    // Codes
    GMLIB::enableLib();
    return true;
}

bool Plugin::disable() {
    logger.info("Disabling GMLIB...");
    // Code for disabling the plugin goes here.
    logger.info("GMLIB Disabled!");
    return true;
}

} // namespace plugin