// Apollo Switch
// StreamProfile.hpp
//
// Core stream profile data structures decoupled from monolithic Settings.

#pragma once

#include <cstdint>
#include <optional>
#include <string>

enum VideoCodec : int { H264, H265, AV1 };
std::string getVideoCodecName(VideoCodec codec);

enum class FramePacingMode : int {
    LOWEST_LATENCY = 0,
    BALANCED = 1,
    SMOOTHEST_VIDEO = 2,
};
std::string getFramePacingModeName(FramePacingMode mode);
const char* getFramePacingModeDebugName(FramePacingMode mode);

enum UpscalingMode : int {
    UPSCALING_OFF = 0,
    UPSCALING_METALFX = 1,
    UPSCALING_FSR1 = 2,
};

enum class StreamProfileContext : int {
    LocalWifi = 0,
    LocalEthernet = 1,
    Remote = 2,
};

struct StreamProfile {
    bool enabled = false;
    std::optional<int> resolution;
    std::optional<int> nativeResolutionScale;
    std::optional<int> fps;
    std::optional<int> bitrate;
    std::optional<VideoCodec> videoCodec;
    std::optional<bool> requestHdr;
    std::optional<FramePacingMode> framePacingMode;
    std::optional<UpscalingMode> upscalingMode;
    std::optional<bool> dithering;
    std::optional<float> ditheringStrength;
    std::optional<bool> rcas;
    std::optional<float> rcasStrength;
};
