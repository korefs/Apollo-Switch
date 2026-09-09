// Apollo Switch
// platform/switch/OperationMode.cpp

#include "OperationMode.hpp"

#ifdef __SWITCH__
#include <switch.h>
#endif

DeviceMode OperationMode::current() {
#ifdef __SWITCH__
    AppletOperationMode mode = appletGetOperationMode();
    switch (mode) {
    case AppletOperationMode_Handheld:
        return DeviceMode::Handheld;
    case AppletOperationMode_Console:
        return DeviceMode::Docked;
    default:
        break;
    }
#endif
    return DeviceMode::Unknown;
}
