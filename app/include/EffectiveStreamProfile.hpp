// Apollo Switch
// EffectiveStreamProfile.hpp
//
// Represents the fully resolved, immutable snapshot used by the streaming session.
// Composed via the precedence model:
//   Session > Host > Contextual Device > Network > Device Defaults > Global

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
    int mappingLayout = 0;
    std::optional<StreamProfileContext> context;
    bool profileApplied = false;
    bool contextualDeviceProfileApplied = false;
    DeviceMode deviceMode = DeviceMode::Unknown;
};

inline void apply_contextual_device_profile(
    EffectiveStreamProfile& settings, const ContextualDeviceProfile& profile) {
    if (!profile.enabled)
        return;

    if (profile.bitrate && *profile.bitrate >= 500 && *profile.bitrate <= 100000)
        settings.bitrate = *profile.bitrate;
    if (profile.videoCodec && (*profile.videoCodec == H264 ||
                               *profile.videoCodec == H265 ||
                               *profile.videoCodec == AV1))
        settings.videoCodec = *profile.videoCodec;
    if (profile.mappingLayout && *profile.mappingLayout >= 0)
        settings.mappingLayout = *profile.mappingLayout;

    settings.contextualDeviceProfileApplied = true;
}

// Seamless backward-compatibility alias for all streaming pipeline components.
using ResolvedStreamSettings = EffectiveStreamProfile;
