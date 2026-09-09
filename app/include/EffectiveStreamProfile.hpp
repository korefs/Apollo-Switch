// Apollo Switch
// EffectiveStreamProfile.hpp
//
// Represents the fully resolved, immutable snapshot used by the streaming session.
// Composed via the 5-layer precedence model:
//   Session Manual Override > Host Profile Override > Network Defaults > Device Defaults > Global Defaults

#pragma once

#include "StreamProfile.hpp"
#include "DeviceProfile.hpp"
#include <optional>

struct EffectiveStreamProfile {
    int resolution = 720;
    int nativeResolutionScale = 100;
    int fps = 60;
    int bitrate = 10000;
    VideoCodec videoCodec = H265;
    bool requestHdr = false;
    FramePacingMode framePacingMode = FramePacingMode::BALANCED;
    UpscalingMode upscalingMode = UPSCALING_OFF;
    bool dithering = false;
    float ditheringStrength = 3.0f;
    bool rcas = true;
    float rcasStrength = 0.2f;
    std::optional<StreamProfileContext> context;
    bool profileApplied = false;
    DeviceMode deviceMode = DeviceMode::Unknown;
};

// Seamless backward-compatibility alias for all streaming pipeline components.
using ResolvedStreamSettings = EffectiveStreamProfile;
