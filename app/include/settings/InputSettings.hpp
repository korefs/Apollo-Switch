// Apollo Switch
// settings/InputSettings.hpp
//
// Controller, touch, mouse, and shortcut input configuration domain.

#pragma once

#include <borealis.hpp>
#include <map>
#include <string>
#include <vector>

enum KeyboardType : int { COMPACT, FULLSIZED };
enum class ButtonOverrideType : int { NONE, SCREENSHOT, HOME };

struct KeyMappingLayout {
    std::string title;
    bool editable;
    std::map<int, int> mapping;
};

struct KeyComboOptions {
    int holdTime;
    std::vector<brls::ControllerButton> buttons;
};

enum class KeyComboAction : int {
    GUIDE,
    OVERLAY,
    MOUSE_INPUT,
    HOST_CLOSE_APP,
    HOST_SWITCH_WINDOW,
};

struct InputSettings {
    bool clickByTap = false;
    bool swapUiKeys = false;
    bool swapJoyconStickToDpad = false;
    bool touchscreenMouseMode = false;
    bool swapMouseKeys = false;
    bool swapMouseScroll = false;
    int mouseSpeedMultiplier = 34;
    int rumbleForce = 100;
    float deadzoneStickLeft = 0.0f;
    float deadzoneStickRight = 0.0f;
    KeyboardType keyboardType = COMPACT;
    int keyboardFingers = 3;
    int keyboardLocale = 0;
    ButtonOverrideType overlaySystemButton = ButtonOverrideType::NONE;
    ButtonOverrideType guideSystemButton = ButtonOverrideType::NONE;
    int currentMappingLayout = 0;
    std::vector<KeyMappingLayout> mappingLayouts;
    KeyComboOptions guideKeyOptions{.holdTime = 0, .buttons = {}};
    KeyComboOptions overlayOptions{
        .holdTime = 0,
        .buttons = {brls::ControllerButton::BUTTON_BACK,
                    brls::ControllerButton::BUTTON_START},
    };
    KeyComboOptions mouseInputOptions{.holdTime = 0, .buttons = {}};
    KeyComboOptions hostCloseAppOptions{.holdTime = 1, .buttons = {}};
    KeyComboOptions hostSwitchWindowOptions{.holdTime = 1, .buttons = {}};
};
