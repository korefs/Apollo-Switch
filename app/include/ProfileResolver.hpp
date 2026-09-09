// Apollo Switch
// ProfileResolver.hpp
//
// Composable Profile Resolver with 5-layer precedence model:
//   1. Session manual override
//   2. Host profile override (host.streamProfiles[context])
//   3. Network profile defaults (WiFi: 15 Mbps / Ethernet: 35 Mbps / Remote: 10 Mbps)
//   4. Device profile defaults (Handheld: 720p 60fps / Docked: 1080p 60fps)
//   5. Global defaults (Settings)

#pragma once

#include "EffectiveStreamProfile.hpp"
#include "DeviceProfile.hpp"
#include "NetworkProfile.hpp"
#include "Settings.hpp"
#include <optional>
#include <string>

class ProfileResolver {
public:
    static NetworkConnectionType currentNetworkConnectionType();

    static std::optional<StreamProfileContext>
    resolveContext(const Host& host, const std::string& activeAddress,
                   NetworkConnectionType connectionType);

    // Full 5-layer resolution:
    // Resolves (host, activeAddress, connectionType, deviceMode, sessionOverride)
    static EffectiveStreamProfile
    resolve(const Host& host, const std::string& activeAddress,
            NetworkConnectionType connectionType,
            DeviceMode deviceMode = DeviceMode::Unknown,
            const std::optional<StreamProfile>& sessionOverride = std::nullopt);

    // Resolve for a specific context (used in UI / previewing per-context profiles)
    static EffectiveStreamProfile
    resolve(const Host& host, StreamProfileContext context,
            DeviceMode deviceMode = DeviceMode::Unknown,
            const std::optional<StreamProfile>& sessionOverride = std::nullopt);

    // Baseline layers
    static EffectiveStreamProfile globalDefaults();
    static StreamProfile deviceDefaults(DeviceMode mode);
    static StreamProfile networkDefaults(NetworkConnectionType type);
    static StreamProfile networkDefaults(StreamProfileContext context);

    // Layer application helpers
    static void applyDeviceDefaults(EffectiveStreamProfile& settings, DeviceMode deviceMode);
    static void applyNetworkDefaults(EffectiveStreamProfile& settings, NetworkConnectionType type);
    static void applyNetworkDefaults(EffectiveStreamProfile& settings, StreamProfileContext context);
    static void applyProfileOverride(EffectiveStreamProfile& settings, const StreamProfile& profile);

    static StreamProfile profileFrom(const EffectiveStreamProfile& settings);
    static std::string contextName(StreamProfileContext context);
    static std::string deviceModeName(DeviceMode mode);
    static std::string summary(const EffectiveStreamProfile& settings);
};

// Pipeline session settings hooks
const ResolvedStreamSettings* activeResolvedStreamSettings();
void setActiveResolvedStreamSettings(const ResolvedStreamSettings* settings);
