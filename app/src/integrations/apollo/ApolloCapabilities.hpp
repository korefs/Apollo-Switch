// Apollo Switch
// integrations/apollo/ApolloCapabilities.hpp
//
// Capability model for Apollo / Sunshine streaming servers.
// Invariant: Never assume a capability. Feature UI is strictly capability-driven.

#pragma once

#include <string>
#include <vector>
#include <utility>

struct _SERVER_DATA;
using SERVER_DATA = struct _SERVER_DATA;

struct ApolloCapabilities {
    // True if the server is identified as Apollo/Sunshine (vs stock GFE)
    bool isApollo = false;

    // Feature capabilities
    bool virtualDisplay = false;
    bool virtualDisplayResolutionControl = false;
    bool virtualDisplayRefreshRateControl = false;
    bool serverCommands = false;
    bool clientPermissions = false;
    bool clipboard = false;
    bool inputOnlyMode = false;
    bool hdr = false;

    // Supported parameters
    std::vector<std::string> supportedCodecs;
    std::vector<std::pair<int, int>> supportedResolutions;
    std::vector<int> supportedRefreshRates;

    // Capability detection from server query response
    static ApolloCapabilities detect(const SERVER_DATA& server);
    static bool isApolloServer(const SERVER_DATA& server);
};
