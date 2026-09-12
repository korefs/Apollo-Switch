#pragma once

#include "Singleton.hpp"
#include <borealis.hpp>
#include <map>
#include <optional>
#include <cstdio>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "StreamProfile.hpp"
#include "DeviceProfile.hpp"
#include "ApolloCapabilities.hpp"
#include "ApolloVirtualDisplay.hpp"

#include "settings/HostSettings.hpp"
#include "settings/StreamSettings.hpp"
#include "settings/VideoSettings.hpp"
#include "settings/AudioSettings.hpp"
#include "settings/InputSettings.hpp"
#include "settings/ApolloSettings.hpp"

class Settings : public Singleton<Settings> {
  public:
    [[nodiscard]] std::string working_dir() const { return m_working_dir; }

    void set_working_dir(const std::string& working_dir);

    [[nodiscard]] std::string launch_path() const { return m_launch_path; }
    void set_launch_path(const std::string& launch_path) { m_launch_path = launch_path; }

    [[nodiscard]] std::string key_dir() const { return m_key_dir; }

    [[nodiscard]] std::string boxart_dir() const { return m_boxart_dir; }

    [[nodiscard]] std::string log_path() const { return m_log_path; }

    [[nodiscard]] std::string gamepad_mapping_path() const { return m_gamepad_mapping_path; }

    [[nodiscard]] std::vector<Host> hosts() const { return m_hosts; }

    [[nodiscard]] std::optional<Host> host(const Host& host) const;

    void add_host(const Host& host);
    void remove_host(const Host& host);
    void set_stream_profile(const Host& host, StreamProfileContext context,
                            const StreamProfile& profile,
                            bool persist = true);
    void set_stream_quality_aggregate(
        const Host& host, StreamProfileContext context,
        const StreamQualityAggregate& aggregate);
    [[nodiscard]] const ContextualDeviceProfile&
    device_profile(DeviceMode mode) const;
    void set_device_profile(DeviceMode mode,
                            const ContextualDeviceProfile& profile,
                            bool persist = true);
    void remove_mapping_layout(int index, bool persist = true);
    [[nodiscard]] bool has_mapping_layout(int index) const;

    void add_favorite(const Host& host, const App& app);
    void remove_favorite(const Host& host, int app_id);
    bool is_favorite(const Host& host, int app_id);
    bool has_any_favorite();

    void set_host_capabilities(const Host& host, const ApolloCapabilities& caps) {
        for (auto& h : m_hosts) {
            if (hosts_match(h, host)) {
                h.apolloCaps = caps;
                break;
            }
        }
    }

    [[nodiscard]] bool has_any_apollo_host() const {
        for (const auto& h : m_hosts) {
            if (h.apolloCaps.has_value() && h.apolloCaps->isApollo) {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] VirtualDisplayMode virtual_display_mode() const {
        return m_virtual_display_mode;
    }
    void set_virtual_display_mode(VirtualDisplayMode mode) {
        m_virtual_display_mode = mode;
    }

    [[nodiscard]] int virtual_display_resolution() const {
        return m_apollo.virtualDisplayResolution;
    }
    void set_virtual_display_resolution(int resolution) {
        m_apollo.virtualDisplayResolution = resolution;
    }

    [[nodiscard]] int virtual_display_custom_width() const {
        return m_apollo.virtualDisplayCustomWidth;
    }
    void set_virtual_display_custom_width(int width) {
        m_apollo.virtualDisplayCustomWidth = width;
    }

    [[nodiscard]] int virtual_display_custom_height() const {
        return m_apollo.virtualDisplayCustomHeight;
    }
    void set_virtual_display_custom_height(int height) {
        m_apollo.virtualDisplayCustomHeight = height;
    }

    [[nodiscard]] int virtual_display_refresh_rate() const {
        return m_apollo.virtualDisplayRefreshRate;
    }
    void set_virtual_display_refresh_rate(int rate) {
        m_apollo.virtualDisplayRefreshRate = rate;
    }

    [[nodiscard]] VideoSettings& video() { return m_video; }
    [[nodiscard]] const VideoSettings& video() const { return m_video; }
    [[nodiscard]] AudioSettings& audio() { return m_audio; }
    [[nodiscard]] const AudioSettings& audio() const { return m_audio; }
    [[nodiscard]] InputSettings& input() { return m_input; }
    [[nodiscard]] const InputSettings& input() const { return m_input; }
    [[nodiscard]] ApolloSettings& apollo() { return m_apollo; }
    [[nodiscard]] const ApolloSettings& apollo() const { return m_apollo; }

    [[nodiscard]] int resolution() const { return m_resolution; }
    void set_resolution(int resolution) { m_resolution = resolution; }

    [[nodiscard]] int native_resolution_scale() const {
        return m_native_resolution_scale;
    }
    void set_native_resolution_scale(int native_resolution_scale) {
        switch (native_resolution_scale) {
        case 50:
        case 75:
        case 100:
        case 200:
            m_native_resolution_scale = native_resolution_scale;
            break;
        default:
            m_native_resolution_scale = 100;
            break;
        }
    }

    [[nodiscard]] int fps() const { return m_fps; }
    void set_fps(int fps) { m_fps = fps; }

    [[nodiscard]] VideoCodec video_codec() const { return m_video_codec; }
    void set_video_codec(VideoCodec video_codec) {
        switch (video_codec) {
        case H264:
        case H265:
        case AV1:
            m_video_codec = video_codec;
            break;
        default:
#ifdef __PSV__
            m_video_codec = H264;
#else
            m_video_codec = H265;
#endif
            break;
        }
    }

    [[nodiscard]] FramePacingMode frame_pacing_mode() const {
#ifdef __SWITCH__
        return m_frame_pacing_mode;
#else
        return FramePacingMode::BALANCED;
#endif
    }
    void set_frame_pacing_mode(FramePacingMode mode) {
        switch (mode) {
        case FramePacingMode::LOWEST_LATENCY:
        case FramePacingMode::BALANCED:
        case FramePacingMode::SMOOTHEST_VIDEO:
            m_frame_pacing_mode = mode;
            break;
        default:
            m_frame_pacing_mode = FramePacingMode::BALANCED;
            break;
        }
    }

    [[nodiscard]] AudioBackend audio_backend() const { return m_audio_backend; }
    void set_audio_backend(AudioBackend audio_backend) { m_audio_backend = audio_backend; }

    [[nodiscard]] int bitrate() const { return m_bitrate; }
    void set_bitrate(int bitrate) { m_bitrate = bitrate; }

    [[nodiscard]] bool request_hdr() const { 
#ifdef SUPPORT_HDR
        return m_enable_hdr; 
#else
        return false; 
#endif
    }
    void set_request_hdr(bool request_hdr) { m_enable_hdr = request_hdr; }

    [[nodiscard]] bool upscaling() const {
#ifdef SUPPORT_UPSCALING
        return m_upscaling_mode != UPSCALING_OFF;
#else
        return false;
#endif
    }
    void set_upscaling(bool upscaling) {
#if defined(PLATFORM_APPLE) && !defined(PLATFORM_TVOS)
        m_upscaling_mode = upscaling ? UPSCALING_METALFX : UPSCALING_OFF;
#else
        m_upscaling_mode = upscaling ? UPSCALING_FSR1 : UPSCALING_OFF;
#endif
    }
    [[nodiscard]] UpscalingMode upscaling_mode() const {
#ifdef SUPPORT_UPSCALING
#if defined(PLATFORM_TVOS)
        return m_upscaling_mode == UPSCALING_OFF ? UPSCALING_OFF : UPSCALING_FSR1;
#else
        return m_upscaling_mode;
#endif
#else
        return UPSCALING_OFF;
#endif
    }
    void set_upscaling_mode(UpscalingMode mode) {
#if defined(PLATFORM_APPLE) && !defined(PLATFORM_TVOS)
        if (mode == UPSCALING_METALFX || mode == UPSCALING_FSR1)
            m_upscaling_mode = mode;
        else
            m_upscaling_mode = UPSCALING_OFF;
#else
        m_upscaling_mode = mode == UPSCALING_OFF ? UPSCALING_OFF : UPSCALING_FSR1;
#endif
    }

        [[nodiscard]] bool dithering() const {
        #ifdef SUPPORT_UPSCALING
            return m_enable_dithering;
        #else
            return false;
        #endif
        }
        void set_dithering(bool dithering) { m_enable_dithering = dithering; }

        [[nodiscard]] float dithering_strength() const {
        #ifdef SUPPORT_UPSCALING
            return static_cast<float>(m_dithering_strength);
        #else
            return 3.0f;
        #endif
        }
        void set_dithering_strength(float strength) {
            if (strength < 1.0f)
                strength = 1.0f;
            else if (strength > 10.0f)
                strength = 10.0f;

            m_dithering_strength = static_cast<int>(strength + 0.5f);
        }

        [[nodiscard]] bool rcas() const {
        #ifdef SUPPORT_UPSCALING
            return m_enable_rcas;
        #else
            return false;
        #endif
        }
        void set_rcas(bool rcas) { m_enable_rcas = rcas; }

        [[nodiscard]] float rcas_strength() const {
        #ifdef SUPPORT_UPSCALING
            return static_cast<float>(m_rcas_strength) / 100.0f;
        #else
            return 0.2f;
        #endif
        }
        void set_rcas_strength(float strength) {
            if (strength < 0.0f)
                strength = 0.0f;
            else if (strength > 1.0f)
                strength = 1.0f;

            m_rcas_strength = static_cast<int>(strength * 100.0f);
        }

    [[nodiscard]] bool click_by_tap() const { return m_click_by_tap; }
    void set_click_by_tap(bool click_by_tap) { m_click_by_tap = click_by_tap; }

    void set_decoder_threads(int decoder_threads) { m_decoder_threads = decoder_threads; }
    [[nodiscard]] int decoder_threads() const { return m_decoder_threads; }

    void set_frames_queue_size(int frames_queue_size) { m_frames_queue_size = frames_queue_size; }
    [[nodiscard]] int frames_queue_size() const { return m_frames_queue_size; }

    void set_sops(bool sops) { m_sops = sops; }
    [[nodiscard]] bool sops() const { return m_sops; }

    void set_play_audio(bool play_audio) { m_play_audio = play_audio; }
    [[nodiscard]] bool play_audio() const { return m_play_audio; }

    void set_write_log(bool write_log) { m_write_log = write_log; }
    [[nodiscard]] bool write_log() const { return m_write_log; }

    void set_swap_ui_keys(bool swap_ui_keys) { m_swap_ui_keys = swap_ui_keys; }
    [[nodiscard]] bool swap_ui_keys() const { return m_swap_ui_keys; }

    void set_swap_joycon_stick_to_dpad(bool value) { m_swap_joycon_stick_to_dpad = value; }
    [[nodiscard]] bool swap_joycon_stick_to_dpad() const { return m_swap_joycon_stick_to_dpad; }

    void set_swap_mouse_keys(bool swap_mouse_keys) { m_swap_mouse_keys = swap_mouse_keys; }
    [[nodiscard]] bool touchscreen_mouse_mode() const { return m_touchscreen_mouse_mode; }

    void set_touchscreen_mouse_mode(bool touchscreen_mouse_mode) { m_touchscreen_mouse_mode = touchscreen_mouse_mode; }
    [[nodiscard]] bool swap_mouse_keys() const { return m_swap_mouse_keys; }

    void set_swap_mouse_scroll(bool swap_mouse_scroll) { m_swap_mouse_scroll = swap_mouse_scroll; }
    [[nodiscard]] bool swap_mouse_scroll() const { return m_swap_mouse_scroll; }

    void set_guide_key_options(KeyComboOptions options) { m_guide_key_options = std::move(options); }
    [[nodiscard]] KeyComboOptions guide_key_options() const { return m_guide_key_options; }

    void set_overlay_options(KeyComboOptions options) { m_overlay_options = std::move(options); }
    [[nodiscard]] KeyComboOptions overlay_options() const { return m_overlay_options; }

    void set_mouse_input_options(KeyComboOptions options) { m_mouse_input_options = std::move(options); }
    [[nodiscard]] KeyComboOptions mouse_input_options() const { return m_mouse_input_options; }

    void set_key_combo_options(KeyComboAction action, KeyComboOptions options);
    [[nodiscard]] KeyComboOptions
    key_combo_options(KeyComboAction action) const;
    [[nodiscard]] std::optional<KeyComboAction> key_combo_conflict(
        KeyComboAction action,
        const std::vector<brls::ControllerButton>& buttons) const;

    void set_volume_amplification(bool allow) { m_volume_amplification = allow; }
    [[nodiscard]] bool get_volume_amplification() const { return m_volume_amplification; }

    void set_volume(int volume) { m_volume = volume; }
    [[nodiscard]] int get_volume() const { return m_volume; }

    void set_use_hw_decoding(bool hw_decoding) { m_use_hw_decoding = hw_decoding; }
    [[nodiscard]] bool use_hw_decoding() const {
#if defined(__linux__) && defined(PLATFORM_DESKTOP)
        return m_use_hw_decoding;
#else
        return true;
#endif
    }

    void set_keyboard_type(KeyboardType type) { m_keyboard_type = type; }
    [[nodiscard]] KeyboardType get_keyboard_type() const { return m_keyboard_type; }

    void set_overlay_system_button(ButtonOverrideType type) { m_overlay_system_button = type; }
    [[nodiscard]] ButtonOverrideType get_overlay_system_button() const { return m_overlay_system_button; }

    void set_guide_system_button(ButtonOverrideType type) { m_guide_system_button = type; }
    [[nodiscard]] ButtonOverrideType get_guide_system_button() const { return m_guide_system_button; }

    void set_keyboard_fingers(int fingers) { m_keyboard_fingers = fingers; }
    [[nodiscard]] int get_keyboard_fingers() const { return m_keyboard_fingers; }

    void set_keyboard_locale(int locale) { m_keyboard_locale = locale; }
    [[nodiscard]] int get_keyboard_locale() const { return m_keyboard_locale; }

    void set_rumble_force(float rumble_force) { m_rumble_force = int(rumble_force * 100); }
    [[nodiscard]] float get_rumble_force() const { return float(m_rumble_force) / 100.f; }

    void set_mouse_speed_multiplier(int mouse_speed_multiplier) { m_mouse_speed_multiplier = mouse_speed_multiplier; }
    [[nodiscard]] int get_mouse_speed_multiplier() const { return m_mouse_speed_multiplier; }

    void set_deadzone_stick_left(float deadzone) { m_deadzone_stick_left = deadzone; }
    [[nodiscard]] float get_deadzone_stick_left() const { return m_deadzone_stick_left; }

    void set_deadzone_stick_right(float deadzone) { m_deadzone_stick_right = deadzone; }
    [[nodiscard]] float get_deadzone_stick_right() const { return m_deadzone_stick_right; }

    int get_current_mapping_layout() const;
    void set_current_mapping_layout(int layout) { m_current_mapping_layout = layout; }

    std::vector<KeyMappingLayout>* get_mapping_laouts() { return &m_mapping_laouts; }

    void load();
    void save();

  private:
    std::string m_working_dir;
    std::string m_launch_path;
    std::string m_key_dir;
    std::string m_boxart_dir;
    std::string m_log_path;
    std::string m_gamepad_mapping_path;

    std::vector<Host> m_hosts;
    VideoSettings m_video;
    AudioSettings m_audio;
    InputSettings m_input;
    ApolloSettings m_apollo;
    ContextualDeviceProfile m_handheld_device_profile;
    ContextualDeviceProfile m_docked_device_profile;
    bool m_write_log_internal = false;

    // Reference aliases maintaining 100% backward compatibility
    int& m_resolution = m_video.resolution;
    int& m_native_resolution_scale = m_video.nativeResolutionScale;
    int& m_fps = m_video.fps;
    VideoCodec& m_video_codec = m_video.videoCodec;
    AudioBackend& m_audio_backend = m_audio.backend;
    int& m_bitrate = m_video.bitrate;
    bool& m_enable_hdr = m_video.requestHdr;
    UpscalingMode& m_upscaling_mode = m_video.upscalingMode;
    bool& m_enable_dithering = m_video.dithering;
    int& m_dithering_strength = m_video.ditheringStrength;
    bool& m_enable_rcas = m_video.rcas;
    int& m_rcas_strength = m_video.rcasStrength;
    bool& m_click_by_tap = m_input.clickByTap;
    int& m_decoder_threads = m_video.decoderThreads;
    int& m_frames_queue_size = m_video.framesQueueSize;
    FramePacingMode& m_frame_pacing_mode = m_video.framePacingMode;
    bool& m_sops = m_video.sops;
    bool& m_play_audio = m_audio.playAudio;
    bool& m_write_log = m_write_log_internal;
    bool& m_swap_ui_keys = m_input.swapUiKeys;
    bool& m_swap_joycon_stick_to_dpad = m_input.swapJoyconStickToDpad;
    bool& m_touchscreen_mouse_mode = m_input.touchscreenMouseMode;
    bool& m_swap_mouse_keys = m_input.swapMouseKeys;
    bool& m_swap_mouse_scroll = m_input.swapMouseScroll;
    int& m_rumble_force = m_input.rumbleForce;
    int& m_volume = m_audio.volume;
    bool& m_use_hw_decoding = m_video.useHwDecoding;
    KeyboardType& m_keyboard_type = m_input.keyboardType;
    ButtonOverrideType& m_overlay_system_button = m_input.overlaySystemButton;
    ButtonOverrideType& m_guide_system_button = m_input.guideSystemButton;
    int& m_keyboard_fingers = m_input.keyboardFingers;
    int& m_keyboard_locale = m_input.keyboardLocale;
    bool& m_volume_amplification = m_audio.volumeAmplification;
    int& m_mouse_speed_multiplier = m_input.mouseSpeedMultiplier;
    int& m_current_mapping_layout = m_input.currentMappingLayout;
    std::vector<KeyMappingLayout>& m_mapping_laouts = m_input.mappingLayouts;
    KeyComboOptions& m_guide_key_options = m_input.guideKeyOptions;
    KeyComboOptions& m_overlay_options = m_input.overlayOptions;
    KeyComboOptions& m_mouse_input_options = m_input.mouseInputOptions;
    KeyComboOptions& m_host_close_app_options = m_input.hostCloseAppOptions;
    KeyComboOptions& m_host_switch_window_options = m_input.hostSwitchWindowOptions;
    float& m_deadzone_stick_left = m_input.deadzoneStickLeft;
    float& m_deadzone_stick_right = m_input.deadzoneStickRight;
    VirtualDisplayMode& m_virtual_display_mode = m_apollo.virtualDisplayMode;

    void loadBaseLayouts();
    void sanitizeHostShortcutOptions();
};
