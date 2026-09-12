// Apollo Switch
// ProfileResolver.cpp

#include "ProfileResolver.hpp"
#include "NetworkState.hpp"
#include "VideoCodecSupport.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

using namespace brls::literals;

namespace {
const ResolvedStreamSettings* activeSettings = nullptr;

bool isValidResolution(int value) {
    return value == -1 || value == 360 || value == 480 || value == 540 ||
           value == 720 || value == 1080 || value == 1440;
}

bool isValidScale(int value) {
    return value == 50 || value == 75 || value == 100 || value == 200;
}

bool isValidFps(int value) {
    return value == 24 || value == 30 || value == 40 || value == 50 ||
           value == 60 || value == 120;
}

bool isValidFramePacing(FramePacingMode value) {
    return value == FramePacingMode::LOWEST_LATENCY ||
           value == FramePacingMode::BALANCED ||
           value == FramePacingMode::SMOOTHEST_VIDEO;
}

bool isValidUpscaling(UpscalingMode value) {
    return value == UPSCALING_OFF || value == UPSCALING_METALFX ||
           value == UPSCALING_FSR1;
}
}

const ResolvedStreamSettings* activeResolvedStreamSettings() {
    return activeSettings;
}

void setActiveResolvedStreamSettings(const ResolvedStreamSettings* settings) {
    activeSettings = settings;
}

NetworkConnectionType ProfileResolver::currentNetworkConnectionType() {
    return NetworkState::current();
}

std::optional<StreamProfileContext> ProfileResolver::resolveContext(
    const Host& host, const std::string& activeAddress,
    NetworkConnectionType connectionType) {
    if (activeAddress.empty())
        return std::nullopt;

    if (!host.remoteAddress.empty() && host.remoteAddress != host.address &&
        activeAddress == host.remoteAddress) {
        return StreamProfileContext::Remote;
    }

    if (host.address.empty() || activeAddress != host.address)
        return std::nullopt;

    if (connectionType == NetworkConnectionType::WIFI)
        return StreamProfileContext::LocalWifi;
    if (connectionType == NetworkConnectionType::ETHERNET)
        return StreamProfileContext::LocalEthernet;
    return std::nullopt;
}

EffectiveStreamProfile ProfileResolver::globalDefaults() {
    const auto& settings = Settings::instance();
    EffectiveStreamProfile resolved;
    resolved.resolution = settings.resolution();
    resolved.nativeResolutionScale = settings.native_resolution_scale();
    resolved.fps = settings.fps();
    resolved.bitrate = settings.bitrate();
    resolved.videoCodec = validatedVideoCodec(settings.video_codec());
    resolved.requestHdr = settings.request_hdr();
    resolved.framePacingMode = settings.frame_pacing_mode();
    resolved.upscalingMode = settings.upscaling_mode();
    resolved.dithering = settings.dithering();
    resolved.ditheringStrength = settings.dithering_strength();
    resolved.rcas = settings.rcas();
    resolved.rcasStrength = settings.rcas_strength();
    resolved.mappingLayout = settings.get_current_mapping_layout();
    return resolved;
}

StreamProfile ProfileResolver::deviceDefaults(DeviceMode mode) {
    return DeviceProfile::forMode(mode);
}

StreamProfile ProfileResolver::networkDefaults(NetworkConnectionType type) {
    return NetworkProfile::forType(type);
}

StreamProfile ProfileResolver::networkDefaults(StreamProfileContext context) {
    return NetworkProfile::forContext(context);
}

void ProfileResolver::applyDeviceDefaults(EffectiveStreamProfile& settings, DeviceMode deviceMode) {
    if (deviceMode == DeviceMode::Unknown)
        return;

    const StreamProfile dev = deviceDefaults(deviceMode);
    if (dev.resolution && isValidResolution(*dev.resolution))
        settings.resolution = *dev.resolution;
    if (dev.fps && isValidFps(*dev.fps))
        settings.fps = *dev.fps;
    settings.deviceMode = deviceMode;
}

void ProfileResolver::applyNetworkDefaults(EffectiveStreamProfile& settings, NetworkConnectionType type) {
    if (type == NetworkConnectionType::UNKNOWN)
        return;

    const StreamProfile net = networkDefaults(type);
    if (net.bitrate && *net.bitrate >= 500 && *net.bitrate <= 150000)
        settings.bitrate = *net.bitrate;
    if (net.fps && isValidFps(*net.fps))
        settings.fps = *net.fps;
}

void ProfileResolver::applyNetworkDefaults(EffectiveStreamProfile& settings, StreamProfileContext context) {
    const StreamProfile net = networkDefaults(context);
    if (net.bitrate && *net.bitrate >= 500 && *net.bitrate <= 150000)
        settings.bitrate = *net.bitrate;
    if (net.fps && isValidFps(*net.fps))
        settings.fps = *net.fps;
}

void ProfileResolver::applyProfileOverride(EffectiveStreamProfile& settings, const StreamProfile& profile) {
    if (!profile.enabled)
        return;

    if (profile.resolution && isValidResolution(*profile.resolution))
        settings.resolution = *profile.resolution;
    if (profile.nativeResolutionScale && isValidScale(*profile.nativeResolutionScale))
        settings.nativeResolutionScale = *profile.nativeResolutionScale;
    if (profile.fps && isValidFps(*profile.fps))
        settings.fps = *profile.fps;
    if (profile.bitrate && *profile.bitrate >= 500 && *profile.bitrate <= 150000)
        settings.bitrate = *profile.bitrate;
    if (profile.videoCodec && (*profile.videoCodec == H264 ||
                               *profile.videoCodec == H265 ||
                               *profile.videoCodec == AV1))
        settings.videoCodec = validatedVideoCodec(*profile.videoCodec);
    if (profile.requestHdr)
        settings.requestHdr = *profile.requestHdr;
    if (profile.framePacingMode && isValidFramePacing(*profile.framePacingMode))
        settings.framePacingMode = *profile.framePacingMode;
    if (profile.upscalingMode && isValidUpscaling(*profile.upscalingMode))
        settings.upscalingMode = *profile.upscalingMode;
    if (profile.dithering)
        settings.dithering = *profile.dithering;
    if (profile.ditheringStrength && *profile.ditheringStrength >= 1.0f &&
        *profile.ditheringStrength <= 10.0f)
        settings.ditheringStrength = *profile.ditheringStrength;
    if (profile.rcas)
        settings.rcas = *profile.rcas;
    if (profile.rcasStrength && *profile.rcasStrength >= 0.0f &&
        *profile.rcasStrength <= 1.0f)
        settings.rcasStrength = *profile.rcasStrength;

    settings.profileApplied = true;
}

EffectiveStreamProfile ProfileResolver::resolve(
    const Host& host, const std::string& activeAddress,
    NetworkConnectionType connectionType, DeviceMode deviceMode,
    const std::optional<StreamProfile>& sessionOverride) {

    // Layer 6: Global Defaults (Settings baseline)
    EffectiveStreamProfile resolved = globalDefaults();

    // Layer 5: Device Profile Defaults
    applyDeviceDefaults(resolved, deviceMode);

    // Layer 4: Network Profile Defaults
    const auto context = resolveContext(host, activeAddress, connectionType);
    resolved.context = context;
    if (context) {
        applyNetworkDefaults(resolved, *context);
    } else {
        applyNetworkDefaults(resolved, connectionType);
    }

    // Layer 3: Contextual Device Profile
    if (deviceMode != DeviceMode::Unknown)
        apply_contextual_device_profile(
            resolved, Settings::instance().device_profile(deviceMode));

    // Layer 2: Host Profile Override (per context)
    if (context) {
        const auto it = host.streamProfiles.find(*context);
        if (it != host.streamProfiles.end() && it->second.enabled) {
            applyProfileOverride(resolved, it->second);
        }
    }

    // Layer 1: Session Manual Override (highest precedence)
    if (sessionOverride && sessionOverride->enabled) {
        applyProfileOverride(resolved, *sessionOverride);
    }

    // Validate the final choice after every profile layer has been composed.
    resolved.videoCodec = validatedVideoCodec(resolved.videoCodec);

    // Hardware capability sanitization
#ifndef SUPPORT_HDR
    resolved.requestHdr = false;
#endif
#ifndef SUPPORT_UPSCALING
    resolved.upscalingMode = UPSCALING_OFF;
    resolved.dithering = false;
    resolved.rcas = false;
#endif

    return resolved;
}

EffectiveStreamProfile ProfileResolver::resolve(
    const Host& host, StreamProfileContext context,
    DeviceMode deviceMode,
    const std::optional<StreamProfile>& sessionOverride) {

    // Layer 6: Global Defaults
    EffectiveStreamProfile resolved = globalDefaults();
    resolved.context = context;

    // Layer 5: Device Profile Defaults
    applyDeviceDefaults(resolved, deviceMode);

    // Layer 4: Network Profile Defaults (for this context)
    applyNetworkDefaults(resolved, context);

    // Layer 3: Contextual Device Profile
    if (deviceMode != DeviceMode::Unknown)
        apply_contextual_device_profile(
            resolved, Settings::instance().device_profile(deviceMode));

    // Layer 2: Host Profile Override
    const auto it = host.streamProfiles.find(context);
    if (it != host.streamProfiles.end() && it->second.enabled) {
        applyProfileOverride(resolved, it->second);
    }

    // Layer 1: Session Manual Override
    if (sessionOverride && sessionOverride->enabled) {
        applyProfileOverride(resolved, *sessionOverride);
    }

    // Validate the final choice after every profile layer has been composed.
    resolved.videoCodec = validatedVideoCodec(resolved.videoCodec);

#ifndef SUPPORT_HDR
    resolved.requestHdr = false;
#endif
#ifndef SUPPORT_UPSCALING
    resolved.upscalingMode = UPSCALING_OFF;
    resolved.dithering = false;
    resolved.rcas = false;
#endif

    return resolved;
}

StreamProfile ProfileResolver::profileFrom(const EffectiveStreamProfile& settings) {
    StreamProfile profile;
    profile.enabled = true;
    profile.resolution = settings.resolution;
    profile.nativeResolutionScale = settings.nativeResolutionScale;
    profile.fps = settings.fps;
    profile.bitrate = settings.bitrate;
    profile.videoCodec = settings.videoCodec;
    profile.requestHdr = settings.requestHdr;
    profile.framePacingMode = settings.framePacingMode;
    profile.upscalingMode = settings.upscalingMode;
    profile.dithering = settings.dithering;
    profile.ditheringStrength = settings.ditheringStrength;
    profile.rcas = settings.rcas;
    profile.rcasStrength = settings.rcasStrength;
    return profile;
}

std::string ProfileResolver::contextName(StreamProfileContext context) {
    switch (context) {
    case StreamProfileContext::LocalWifi:
        return "stream_profiles/local_wifi"_i18n;
    case StreamProfileContext::LocalEthernet:
        return "stream_profiles/local_ethernet"_i18n;
    case StreamProfileContext::Remote:
        return "stream_profiles/remote"_i18n;
    }
    return {};
}

std::string ProfileResolver::deviceModeName(DeviceMode mode) {
    return DeviceProfile::modeName(mode);
}

std::string ProfileResolver::summary(const EffectiveStreamProfile& settings) {
    const std::string resolution = settings.resolution == -1
                                       ? "settings/resolution_native"_i18n
                                       : std::to_string(settings.resolution) + "p";
    std::ostringstream bitrate;
    bitrate << std::lround(settings.bitrate / 1000.0f) << " Mbps";
    return resolution + " · " + std::to_string(settings.fps) + " FPS · " +
           bitrate.str();
}
