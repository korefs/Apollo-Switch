// Apollo Switch
// settings/AudioSettings.hpp
//
// Audio playback and backend configuration domain.

#pragma once

enum AudioBackend : int {
    SDL,
#ifdef __SWITCH__
    AUDREN,
#endif
};

struct AudioSettings {
    int volume = 100;
    bool volumeAmplification = false;
    bool playAudio = false;
    AudioBackend backend =
#ifdef __SWITCH__
        AUDREN;
#else
        SDL;
#endif
};
