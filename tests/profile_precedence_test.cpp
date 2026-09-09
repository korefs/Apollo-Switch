// Apollo Switch
// tests/profile_precedence_test.cpp
//
// Unit tests for the composable 5-layer profile system precedence rules:
//   Session Override > Host Profile > Network Defaults > Device Defaults > Global Defaults

#include "StreamProfile.hpp"
#include "DeviceProfile.hpp"
#include "NetworkProfile.hpp"
#include "EffectiveStreamProfile.hpp"
#include <cassert>
#include <iostream>
#include <map>

// Lightweight mock host for testing precedence without full Settings singleton
struct MockHost {
    std::string address = "192.168.1.100";
    std::string remoteAddress;
    std::map<StreamProfileContext, StreamProfile> streamProfiles;
};

// Pure precedence resolution mirroring ProfileResolver
EffectiveStreamProfile testResolve(
    const EffectiveStreamProfile& baseline,
    const MockHost& host,
    std::optional<StreamProfileContext> context,
    DeviceMode deviceMode,
    const std::optional<StreamProfile>& sessionOverride = std::nullopt) {

    // Layer 5: Baseline (Global Defaults)
    EffectiveStreamProfile resolved = baseline;

    // Layer 4: Device Profile Defaults
    if (deviceMode != DeviceMode::Unknown) {
        StreamProfile dev = DeviceProfile::forMode(deviceMode);
        if (dev.resolution) resolved.resolution = *dev.resolution;
        if (dev.fps) resolved.fps = *dev.fps;
        resolved.deviceMode = deviceMode;
    }

    // Layer 3: Network Profile Defaults
    if (context) {
        StreamProfile net = NetworkProfile::forContext(*context);
        if (net.bitrate) resolved.bitrate = *net.bitrate;
        if (net.fps) resolved.fps = *net.fps;
    }

    // Layer 2: Host Profile Override
    if (context) {
        auto it = host.streamProfiles.find(*context);
        if (it != host.streamProfiles.end() && it->second.enabled) {
            const auto& p = it->second;
            if (p.resolution) resolved.resolution = *p.resolution;
            if (p.fps) resolved.fps = *p.fps;
            if (p.bitrate) resolved.bitrate = *p.bitrate;
            if (p.videoCodec) resolved.videoCodec = *p.videoCodec;
            resolved.profileApplied = true;
        }
    }

    // Layer 1: Session Manual Override (Highest Precedence)
    if (sessionOverride && sessionOverride->enabled) {
        const auto& p = *sessionOverride;
        if (p.resolution) resolved.resolution = *p.resolution;
        if (p.fps) resolved.fps = *p.fps;
        if (p.bitrate) resolved.bitrate = *p.bitrate;
        if (p.videoCodec) resolved.videoCodec = *p.videoCodec;
    }

    return resolved;
}

void testDeviceDefaults() {
    auto handheld = DeviceProfile::forMode(DeviceMode::Handheld);
    assert(handheld.resolution.value_or(0) == 720);
    assert(handheld.fps.value_or(0) == 60);

    auto docked = DeviceProfile::forMode(DeviceMode::Docked);
    assert(docked.resolution.value_or(0) == 1080);
    assert(docked.fps.value_or(0) == 60);

    auto unknown = DeviceProfile::forMode(DeviceMode::Unknown);
    assert(!unknown.enabled);
    assert(!unknown.resolution.has_value());

    std::cout << "[PASS] testDeviceDefaults\n";
}

void testNetworkDefaults() {
    auto wifi = NetworkProfile::forType(NetworkConnectionType::WIFI);
    assert(wifi.bitrate.value_or(0) == 15000);

    auto eth = NetworkProfile::forType(NetworkConnectionType::ETHERNET);
    assert(eth.bitrate.value_or(0) == 35000);

    auto remote = NetworkProfile::forContext(StreamProfileContext::Remote);
    assert(remote.bitrate.value_or(0) == 10000);
    assert(remote.fps.value_or(0) == 30);

    std::cout << "[PASS] testNetworkDefaults\n";
}

void testPrecedenceChain() {
    EffectiveStreamProfile baseline;
    baseline.resolution = 720;
    baseline.fps = 60;
    baseline.bitrate = 10000;
    baseline.videoCodec = VideoCodec::H264;

    MockHost host;

    // 1. Baseline only
    auto res1 = testResolve(baseline, host, std::nullopt, DeviceMode::Unknown);
    assert(res1.resolution == 720);
    assert(res1.bitrate == 10000);
    assert(res1.fps == 60);
    assert(!res1.profileApplied);

    // 2. Apply DeviceProfile Docked (1080p, 60fps)
    auto res2 = testResolve(baseline, host, std::nullopt, DeviceMode::Docked);
    assert(res2.resolution == 1080);
    assert(res2.fps == 60);
    assert(res2.deviceMode == DeviceMode::Docked);

    // 3. Apply NetworkProfile LocalEthernet (35000 kbps) with Docked
    auto res3 = testResolve(baseline, host, StreamProfileContext::LocalEthernet, DeviceMode::Docked);
    assert(res3.resolution == 1080); // from DeviceProfile
    assert(res3.bitrate == 35000);   // from NetworkProfile
    assert(res3.fps == 60);

    // 4. Host Profile override (user configured custom 20000 kbps, 720p for LocalEthernet)
    StreamProfile hostOverride;
    hostOverride.enabled = true;
    hostOverride.bitrate = 20000;
    hostOverride.resolution = 720;
    host.streamProfiles[StreamProfileContext::LocalEthernet] = hostOverride;

    auto res4 = testResolve(baseline, host, StreamProfileContext::LocalEthernet, DeviceMode::Docked);
    assert(res4.resolution == 720);  // overridden by Host Profile
    assert(res4.bitrate == 20000);   // overridden by Host Profile
    assert(res4.profileApplied);

    // 5. Session Manual Override (user temporarily forces 50000 kbps, 1440p, AV1)
    StreamProfile sessionOverride;
    sessionOverride.enabled = true;
    sessionOverride.bitrate = 50000;
    sessionOverride.resolution = 1440;
    sessionOverride.videoCodec = VideoCodec::AV1;

    auto res5 = testResolve(baseline, host, StreamProfileContext::LocalEthernet, DeviceMode::Docked, sessionOverride);
    assert(res5.resolution == 1440);          // overridden by Session Override
    assert(res5.bitrate == 50000);           // overridden by Session Override
    assert(res5.videoCodec == VideoCodec::AV1); // overridden by Session Override

    std::cout << "[PASS] testPrecedenceChain\n";
}

int main() {
    std::cout << "Running Apollo Switch profile precedence tests...\n";
    testDeviceDefaults();
    testNetworkDefaults();
    testPrecedenceChain();
    std::cout << "All profile precedence tests passed successfully!\n";
    return 0;
}
