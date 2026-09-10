// Apollo Switch
// SwitchStreamProvider.hpp
//
// Hardware-accelerated decoding and rendering provider for Nintendo Switch.

#pragma once

#include "IStreamSessionProvider.hpp"

class SwitchStreamProvider : public IStreamSessionProvider {
public:
    SwitchStreamProvider() = default;

    IFFmpegVideoDecoder* video_decoder() override;
    IVideoRenderer* video_renderer() override;
    IAudioRenderer* audio_renderer() override;
};

// Backward-compatibility alias
using SwitchMoonlightSessionDecoderAndRenderProvider = SwitchStreamProvider;
