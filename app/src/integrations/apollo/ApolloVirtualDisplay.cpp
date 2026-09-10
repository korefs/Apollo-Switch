// Apollo Switch
// integrations/apollo/ApolloVirtualDisplay.cpp
//
// Virtual display session configuration builder for Apollo/Sunshine hosts.

#include "ApolloVirtualDisplay.hpp"
#include "Settings.hpp"

#ifdef PLATFORM_SWITCH
#include "platform/switch/OperationMode.hpp"
#endif

VirtualDisplayConfig ApolloVirtualDisplay::buildConfig() {
    const auto& apollo = Settings::instance().apollo();
    VirtualDisplayConfig config;
    config.mode = apollo.virtualDisplayMode;

    if (config.mode == VirtualDisplayMode::Off) {
        return config;
    }

    // Resolve device mode
#ifdef PLATFORM_SWITCH
    DeviceMode deviceMode = OperationMode::current();
#else
    DeviceMode deviceMode = DeviceMode::Docked;
#endif

    // Resolve resolution
    int resolution = apollo.virtualDisplayResolution;
    if (resolution == 0) {
        // Auto: use OperationMode to decide
        auto [w, h] = config.resolvedResolution(deviceMode);
        config.width = w;
        config.height = h;
    } else if (resolution == 720) {
        config.width = 1280;
        config.height = 720;
    } else if (resolution == 1080) {
        config.width = 1920;
        config.height = 1080;
    } else if (resolution == 1440) {
        config.width = 2560;
        config.height = 1440;
    } else {
        // Custom
        config.width = apollo.virtualDisplayCustomWidth > 0
                            ? apollo.virtualDisplayCustomWidth
                            : 1920;
        config.height = apollo.virtualDisplayCustomHeight > 0
                             ? apollo.virtualDisplayCustomHeight
                             : 1080;
    }

    // Resolve refresh rate
    int refreshRate = apollo.virtualDisplayRefreshRate;
    if (refreshRate == 0) {
        config.refreshRate = 60; // Auto defaults to 60 Hz
    } else {
        config.refreshRate = refreshRate;
    }

    return config;
}

VirtualDisplayConfig ApolloVirtualDisplay::buildConfig(
    const ApolloSettings& apollo, DeviceMode deviceMode) {
    VirtualDisplayConfig config;
    config.mode = apollo.virtualDisplayMode;

    if (config.mode == VirtualDisplayMode::Off) {
        return config;
    }

    int resolution = apollo.virtualDisplayResolution;
    if (resolution == 0) {
        auto [w, h] = config.resolvedResolution(deviceMode);
        config.width = w;
        config.height = h;
    } else if (resolution == 720) {
        config.width = 1280;
        config.height = 720;
    } else if (resolution == 1080) {
        config.width = 1920;
        config.height = 1080;
    } else if (resolution == 1440) {
        config.width = 2560;
        config.height = 1440;
    } else {
        config.width = apollo.virtualDisplayCustomWidth > 0
                            ? apollo.virtualDisplayCustomWidth
                            : 1920;
        config.height = apollo.virtualDisplayCustomHeight > 0
                             ? apollo.virtualDisplayCustomHeight
                             : 1080;
    }

    int refreshRate = apollo.virtualDisplayRefreshRate;
    config.refreshRate = (refreshRate > 0) ? refreshRate : 60;

    return config;
}
