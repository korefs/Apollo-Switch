// Apollo Switch
// integrations/apollo/ApolloVirtualDisplay.hpp
//
// Virtual Display configuration model for Apollo hosts.

#pragma once

#include "DeviceProfile.hpp"
#include <optional>
#include <utility>

enum class VirtualDisplayMode : int {
    Off = 0,
    Automatic = 1,
    Always = 2,
};

struct VirtualDisplayConfig {
    VirtualDisplayMode mode = VirtualDisplayMode::Off;
    std::optional<int> width;
    std::optional<int> height;
    std::optional<int> refreshRate;

    // Computes effective virtual display resolution based on mode and console state:
    // - Automatic: Handheld -> 1280x720, Docked -> 1920x1080
    // - Custom / Always: configured width/height, or fallback to 1080p
    std::pair<int, int> resolvedResolution(DeviceMode deviceMode) const {
        if (mode == VirtualDisplayMode::Automatic) {
            switch (deviceMode) {
            case DeviceMode::Handheld:
                return {1280, 720};
            case DeviceMode::Docked:
                return {1920, 1080};
            case DeviceMode::Unknown:
                break;
            }
        }
        if (width && height && *width > 0 && *height > 0) {
            return {*width, *height};
        }
        return {1920, 1080};
    }
};

struct ApolloSettings;

class ApolloVirtualDisplay {
public:
    static const char* modeName(VirtualDisplayMode mode) {
        switch (mode) {
        case VirtualDisplayMode::Off:
            return "Off";
        case VirtualDisplayMode::Automatic:
            return "Automatic";
        case VirtualDisplayMode::Always:
            return "Always";
        }
        return "Off";
    }

    // Build virtual display config from current Settings + OperationMode
    static VirtualDisplayConfig buildConfig();

    // Build virtual display config from explicit parameters
    static VirtualDisplayConfig buildConfig(
        const ApolloSettings& apollo, DeviceMode deviceMode);
};
