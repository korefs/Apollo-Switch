// Apollo Switch
// DeviceProfile.hpp
//
// Encapsulates Nintendo Switch device profiles (Handheld vs. Docked).

#pragma once

#include "StreamProfile.hpp"
#include <string>

enum class DeviceMode : int {
    Unknown  = 0,
    Handheld = 1,
    Docked   = 2,
};

class DeviceProfile {
public:
    // Returns default streaming parameters recommended for the given device mode.
    // - Handheld: 720p 60 FPS (native Switch screen resolution)
    // - Docked: 1080p 60 FPS (standard dock output capability)
    // - Unknown: empty profile (no device-level defaults applied)
    static StreamProfile forMode(DeviceMode mode) {
        StreamProfile profile;
        profile.enabled = true;
        switch (mode) {
        case DeviceMode::Handheld:
            profile.resolution = 720;
            profile.fps = 60;
            break;
        case DeviceMode::Docked:
            profile.resolution = 1080;
            profile.fps = 60;
            break;
        case DeviceMode::Unknown:
            profile.enabled = false;
            break;
        }
        return profile;
    }

    static std::string modeName(DeviceMode mode) {
        switch (mode) {
        case DeviceMode::Handheld:
            return "Handheld";
        case DeviceMode::Docked:
            return "Docked";
        case DeviceMode::Unknown:
            break;
        }
        return "Unknown";
    }
};
