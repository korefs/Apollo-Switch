// Apollo Switch
// integrations/apollo/ApolloCapabilities.cpp

#include "ApolloCapabilities.hpp"
#include "client.h"

bool ApolloCapabilities::isApolloServer(const SERVER_DATA& server) {
    // Apollo is Sunshine-based; Sunshine encodes a negative 4th quad in serverInfoAppVersion.
    return const_cast<SERVER_DATA&>(server).isSunshine();
}

ApolloCapabilities ApolloCapabilities::detect(const SERVER_DATA& server) {
    ApolloCapabilities caps;
    if (!isApolloServer(server)) {
        // Standard GFE host — no Apollo extensions supported
        return caps;
    }

    caps.isApollo = true;
    caps.virtualDisplay = true;
    caps.virtualDisplayResolutionControl = true;
    caps.virtualDisplayRefreshRateControl = true;
    caps.serverCommands = true;
    caps.clientPermissions = false;
    caps.clipboard = false;
    caps.inputOnlyMode = false;
    caps.hdr = true;

    caps.supportedCodecs = {"H.264", "HEVC (H.265)", "AV1"};
    caps.supportedResolutions = {
        {1280, 720},
        {1920, 1080},
        {2560, 1440},
    };
    caps.supportedRefreshRates = {30, 40, 60, 120};

    return caps;
}
