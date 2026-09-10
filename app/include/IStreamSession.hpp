// Apollo Switch
// IStreamSession.hpp
//
// Application-level abstract interface for streaming sessions.

#pragma once

#include "EffectiveStreamProfile.hpp"
#include "IFFmpegVideoDecoder.hpp"
#include "IVideoRenderer.hpp"
#include "GameStreamClient.hpp"
#include <string>

struct NVGcontext;

struct SessionStats {
    VideoDecodeStats video_decode_stats;
    VideoRenderStats video_render_stats;
};

class IStreamSession {
public:
    virtual ~IStreamSession() = default;

    virtual void start(ServerCallback<bool> callback, bool is_sunshine) = 0;
    virtual void stop(int terminate_app) = 0;
    virtual void set_address(const std::string& address) = 0;
    virtual void set_stream_settings(const ResolvedStreamSettings& settings) = 0;
    [[nodiscard]] virtual const ResolvedStreamSettings& stream_settings() const = 0;

    virtual void restart() = 0;
    virtual void draw(NVGcontext* vg, int width, int height) = 0;

    [[nodiscard]] virtual bool is_active() const = 0;
    [[nodiscard]] virtual bool is_terminated() const = 0;
    [[nodiscard]] virtual bool connection_status_is_poor() const = 0;
    [[nodiscard]] virtual bool use_hdr() const = 0;
    [[nodiscard]] virtual SessionStats* session_stats() const = 0;
};
