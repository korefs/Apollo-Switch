// Apollo Switch
// settings/VideoSettings.hpp
//
// Video and display streaming configuration domain.

#pragma once

#include "StreamProfile.hpp"

struct VideoSettings {
    int resolution = 720;
    int nativeResolutionScale = 100;
    int fps = 60;
    int bitrate = 10000;
    VideoCodec videoCodec = H265;
    bool requestHdr = false;
    bool useHwDecoding = true;
    int decoderThreads = 4;
    int framesQueueSize = 3;
    FramePacingMode framePacingMode = FramePacingMode::BALANCED;
    UpscalingMode upscalingMode = UPSCALING_OFF;
    bool dithering = false;
    int ditheringStrength = 3;
    bool rcas = true;
    int rcasStrength = 20;
    bool sops = false;
};
