// Apollo Switch
// platform/switch/NetworkState.cpp

#include "NetworkState.hpp"

#ifdef __SWITCH__
#include <switch.h>
#endif

NetworkConnectionType NetworkState::current() {
#ifdef __SWITCH__
    NifmInternetConnectionType type;
    NifmInternetConnectionStatus status;
    u32 wifiStrength = 0;
    if (R_FAILED(nifmGetInternetConnectionStatus(&type, &wifiStrength,
                                                  &status)) ||
        status != NifmInternetConnectionStatus_Connected) {
        return NetworkConnectionType::UNKNOWN;
    }

    if (type == NifmInternetConnectionType_WiFi)
        return NetworkConnectionType::WIFI;
    if (type == NifmInternetConnectionType_Ethernet)
        return NetworkConnectionType::ETHERNET;
#endif
    return NetworkConnectionType::UNKNOWN;
}
