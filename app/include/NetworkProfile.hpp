// Apollo Switch
// NetworkProfile.hpp
//
// Encapsulates network connection profiles and recommended bandwidth/framerate defaults.

#pragma once

#include "StreamProfile.hpp"
#include <string>

enum class NetworkConnectionType : int {
    UNKNOWN  = 0,
    WIFI     = 1,
    ETHERNET = 2,
};

class NetworkProfile {
public:
    // Default bitrate recommendations based on connection medium:
    // - WiFi: 15 Mbps (tuned for Switch 802.11ac wireless chip)
    // - Ethernet: 35 Mbps (wired USB-Ethernet adapter)
    // - Unknown: empty profile (no network-level defaults applied)
    static StreamProfile forType(NetworkConnectionType type) {
        StreamProfile profile;
        profile.enabled = true;
        switch (type) {
        case NetworkConnectionType::WIFI:
            profile.bitrate = 15000;
            break;
        case NetworkConnectionType::ETHERNET:
            profile.bitrate = 35000;
            break;
        case NetworkConnectionType::UNKNOWN:
            profile.enabled = false;
            break;
        }
        return profile;
    }

    // Default parameters for high-level stream profile contexts:
    // - LocalWifi: 15 Mbps
    // - LocalEthernet: 35 Mbps
    // - Remote: 10 Mbps, 30 FPS (conservative for WAN latency & packet loss)
    static StreamProfile forContext(StreamProfileContext context) {
        StreamProfile profile;
        profile.enabled = true;
        switch (context) {
        case StreamProfileContext::LocalWifi:
            profile.bitrate = 15000;
            break;
        case StreamProfileContext::LocalEthernet:
            profile.bitrate = 35000;
            break;
        case StreamProfileContext::Remote:
            profile.bitrate = 10000;
            profile.fps = 30;
            break;
        }
        return profile;
    }

    static std::string typeName(NetworkConnectionType type) {
        switch (type) {
        case NetworkConnectionType::WIFI:
            return "Wi-Fi";
        case NetworkConnectionType::ETHERNET:
            return "Ethernet";
        case NetworkConnectionType::UNKNOWN:
            break;
        }
        return "Unknown";
    }
};
