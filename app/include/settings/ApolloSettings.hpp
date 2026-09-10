// Apollo Switch
// settings/ApolloSettings.hpp
//
// Apollo host-specific configuration domain.

#pragma once

#include "integrations/apollo/ApolloVirtualDisplay.hpp"

struct ApolloSettings {
    VirtualDisplayMode virtualDisplayMode = VirtualDisplayMode::Off;

    // Virtual display resolution: 0 = Auto (uses OperationMode),
    // or explicit value: 720, 1080, 1440
    int virtualDisplayResolution = 0;

    // Custom resolution (used when virtualDisplayResolution is not a preset)
    int virtualDisplayCustomWidth = 0;
    int virtualDisplayCustomHeight = 0;

    // Virtual display refresh rate: 0 = Auto (60 Hz),
    // or explicit value: 30, 60, 120
    int virtualDisplayRefreshRate = 0;
};
