// Apollo Switch
// IStreamSessionProvider.hpp
//
// Formal interface for hardware/platform decoding and rendering providers.

#pragma once

class IAudioRenderer;
class IFFmpegVideoDecoder;
class IVideoRenderer;

class IStreamSessionProvider {
public:
    virtual ~IStreamSessionProvider() = default;
    virtual IFFmpegVideoDecoder* video_decoder() = 0;
    virtual IVideoRenderer* video_renderer() = 0;
    virtual IAudioRenderer* audio_renderer() = 0;
};

// Backward-compatibility alias
using MoonlightSessionDecoderAndRenderProvider = IStreamSessionProvider;
