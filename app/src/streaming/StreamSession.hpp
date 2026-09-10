#pragma once

#include "IStreamSession.hpp"
#include "GameStreamClient.hpp"
#include "MoonlightSessionDecoderAndRenderProvider.hpp"
#include "IAudioRenderer.hpp"
#include "StreamProfileResolver.hpp"
#include <nanovg.h>

class StreamSession : public IStreamSession {
  public:
    static void
    set_provider(MoonlightSessionDecoderAndRenderProvider* provider);

    StreamSession(const std::string& address, int app_id);
    ~StreamSession() override;

    static StreamSession* activeSession();

    void start(ServerCallback<bool> callback, bool is_sunshine) override;
    void stop(int terminate_app) override;
    void set_address(const std::string& address) override { m_address = address; }
    void set_stream_settings(const ResolvedStreamSettings& settings) override;
    [[nodiscard]] const ResolvedStreamSettings& stream_settings() const override {
        return m_stream_settings;
    }

    void restart() override;

    void draw(NVGcontext* vg, int width, int height) override;

    [[nodiscard]] bool is_active() const override { return m_is_active; }
    [[nodiscard]] bool is_terminated() const override { return m_is_terminated; }

    [[nodiscard]] bool connection_status_is_poor() const override {
        return m_connection_status_is_poor;
    }

    [[nodiscard]] bool use_hdr() const override {
        return m_use_hdr;
    }

    [[nodiscard]] SessionStats* session_stats() const override {
        return (SessionStats*)&m_session_stats;
    }

  private:
    void start_internal(ServerCallback<bool> callback, bool is_sunshine);
    bool fallback_from_av1();

    static void connection_stage_starting(int);
    static void connection_stage_complete(int);
    static void connection_stage_failed(int, int);
    static void connection_started();
    static void connection_terminated(int);
    static void connection_log_message(const char* format, ...);
    static void connection_rumble(unsigned short, unsigned short,
                                  unsigned short);
    static void connection_rumble_triggers(uint16_t controllerNumber,
                                           uint16_t leftTriggerMotor, uint16_t rightTriggerMotor);
    static void connection_status_update(int);
    static void connection_set_hdr_mode(bool);

    static int video_decoder_setup(int, int, int, int, void*, int);
    static void video_decoder_start();
    static void video_decoder_stop();
    static void video_decoder_cleanup();
    static int video_decoder_submit_decode_unit(PDECODE_UNIT);

    static int audio_renderer_init(int, const POPUS_MULTISTREAM_CONFIGURATION,
                                   void*, int);
    static void audio_renderer_start();
    static void audio_renderer_stop();
    static void audio_renderer_cleanup();
    static void audio_renderer_decode_and_play_sample(char*, int);

    std::string m_address;
    int m_app_id;
    bool m_is_sunshine = false;
    STREAM_CONFIGURATION m_config;
    CONNECTION_LISTENER_CALLBACKS m_connection_callbacks;
    DECODER_RENDERER_CALLBACKS m_video_callbacks;
    AUDIO_RENDERER_CALLBACKS m_audio_callbacks;

    IFFmpegVideoDecoder* m_video_decoder = nullptr;
    IVideoRenderer* m_video_renderer = nullptr;
    IAudioRenderer* m_audio_renderer = nullptr;

    bool m_is_active = false;
    bool m_is_terminated = false;
    bool m_stop_requested = false;
    bool m_connection_status_is_poor = false;
    bool m_use_hdr = false;
    bool m_force_disable_av1 = false;
    bool m_av1_fallback_attempted = false;
    bool m_av1_decoder_failed = false;
    ResolvedStreamSettings m_stream_settings;

    SessionStats m_session_stats = {};
    uint64_t m_last_stats_update_ms = 0;
};

// Application-level stream session alias
using MoonlightSession = StreamSession;
