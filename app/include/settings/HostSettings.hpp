// Apollo Switch
// settings/HostSettings.hpp
//
// Host connection and management configuration domain.

#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "StreamProfile.hpp"
#include "integrations/apollo/ApolloCapabilities.hpp"

// Compact, per-host/context storage reserved for the quality assistant. Phase 1
// only persists this model; it does not collect or upload telemetry.
struct StreamQualityAggregate {
    uint32_t sampleCount = 0;
    float rttMs = 0.0f;
    float rttVariationMs = 0.0f;
    float networkFrameLoss = 0.0f;
    float hostFps = 0.0f;
    float receivedFps = 0.0f;
    float decodedFps = 0.0f;
    float renderedFps = 0.0f;
    float decodeTimeMs = 0.0f;
    float gpuTimeMs = 0.0f;
    float postProcessingTimeMs = 0.0f;
    float queueDepth = 0.0f;
    float underflowsPerSecond = 0.0f;
};

struct App {
    std::string name;
    int app_id;
};

struct Host {
    std::string address;
    std::string remoteAddress;
    std::string hostname;
    std::string mac;
    std::vector<App> favorites;
    std::map<StreamProfileContext, StreamProfile> streamProfiles;
    std::map<StreamProfileContext, StreamQualityAggregate> streamQuality;
    std::optional<ApolloCapabilities> apolloCaps;

    [[nodiscard]] std::vector<std::string> connection_addresses() const {
        std::vector<std::string> addresses;
        if (!address.empty())
            addresses.push_back(address);
        if (!remoteAddress.empty() && remoteAddress != address)
            addresses.push_back(remoteAddress);
        return addresses;
    }

    [[nodiscard]] std::string preferred_address() const {
        return !address.empty() ? address : remoteAddress;
    }

    [[nodiscard]] bool has_address(const std::string& value) const {
        return !value.empty() &&
               (address == value || remoteAddress == value);
    }
};

inline bool hosts_match(const Host& lhs, const Host& rhs) {
    if (!lhs.mac.empty() && !rhs.mac.empty())
        return lhs.mac == rhs.mac;

    for (const auto& address : lhs.connection_addresses()) {
        if (rhs.has_address(address))
            return true;
    }

    return false;
}
