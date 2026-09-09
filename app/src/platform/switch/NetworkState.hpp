// Apollo Switch
// platform/switch/NetworkState.hpp
//
// Thin extraction of the nifm-based network connection type detection
// that previously lived inline in StreamProfileResolver.cpp.
//
// Keeping this as a platform façade means StreamProfileResolver is no
// longer conditionally compiled for Switch vs. other platforms for the
// network-detection concern.

#pragma once

#include "NetworkProfile.hpp"

class NetworkState {
public:
    // Returns the current network connection type.
    static NetworkConnectionType current();
};
