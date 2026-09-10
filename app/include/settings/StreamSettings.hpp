// Apollo Switch
// settings/StreamSettings.hpp
//
// Aggregated streaming session configuration domain.
// Groups VideoSettings with stream-level parameters that are commonly resolved
// together during profile resolution and session launch.

#pragma once

#include "settings/VideoSettings.hpp"
#include "settings/AudioSettings.hpp"

struct StreamSettings {
    VideoSettings video;
    AudioSettings audio;
    bool sops = false;           // Use streaming optimal playable settings
    bool playAudio = false;      // Play audio on host PC
};
