// Apollo Switch
// platform/switch/OperationMode.hpp
//
// Thin wrapper around libnx appletGetOperationMode().
// Detects handheld vs. docked mode for profile resolution at session start.
//
// Invariants:
//   - NEVER infers mode from screen resolution.
//   - NEVER called mid-stream to renegotiate; call only at session-start.
//   - Non-Switch builds always return DeviceMode::Unknown.

#pragma once

#include "DeviceProfile.hpp"

class OperationMode {
public:
    // Returns the current Switch operation mode via appletGetOperationMode().
    // Falls back to DeviceMode::Unknown on failure or on non-Switch platforms.
    static DeviceMode current();

    static bool isHandheld() { return current() == DeviceMode::Handheld; }
    static bool isDocked()   { return current() == DeviceMode::Docked;   }
};
