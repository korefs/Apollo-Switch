#include "contextual_profiles_view.hpp"

#include "VideoCodecSupport.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

using namespace brls;
using namespace brls::literals;

namespace {
std::string bitrate_text(int bitrate) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(1) << bitrate / 1000.0f
           << " Mbps";
    return stream.str();
}

std::string profile_state(const ContextualDeviceProfile& profile) {
    return profile.enabled ? "contextual_profiles/configured"_i18n
                           : "contextual_profiles/disabled"_i18n;
}
}

ContextualProfilesView::ContextualProfilesView() {
    inflateFromXMLRes("xml/views/contextual_profiles.xml");

    handheld->setText("contextual_profiles/handheld"_i18n);
    docked->setText("contextual_profiles/docked"_i18n);
    handheld->registerClickAction([this](View*) {
        loadMode(DeviceMode::Handheld);
        return true;
    });
    docked->registerClickAction([this](View*) {
        loadMode(DeviceMode::Docked);
        return true;
    });

    enabled->init("contextual_profiles/enable"_i18n, false, [this](bool value) {
        if (!refreshing) {
            profile.enabled = value;
            saveProfile();
            refreshControls();
        }
    });
    bitrateOverride->init("contextual_profiles/override_bitrate"_i18n, false,
                          [this](bool value) {
                              if (!refreshing) {
                                  if (value)
                                      profile.bitrate = Settings::instance().bitrate();
                                  else
                                      profile.bitrate.reset();
                                  saveProfile();
                                  refreshControls();
                              }
                          });
    bitrateSlider->getProgressEvent()->subscribe([this](float progress) {
        if (!refreshing) {
            profile.bitrate = static_cast<int>(progress * 99500.0f + 500.0f);
            saveProfile();
            refreshControls();
        }
    });

    codecs = {H264, H265};
    if (isAv1HardwareDecodingAvailable())
        codecs.push_back(AV1);
    std::vector<std::string> codecNames = {"contextual_profiles/inherit"_i18n};
    for (VideoCodec value : codecs)
        codecNames.push_back(getVideoCodecName(value));
    codec->init("settings/video_codec"_i18n, codecNames, 0, [this](int selected) {
        if (!refreshing && selected >= 0 &&
            static_cast<size_t>(selected) <= codecs.size()) {
            if (selected == 0)
                profile.videoCodec.reset();
            else
                profile.videoCodec = codecs[selected - 1];
            saveProfile();
        }
    });

    std::vector<std::string> layouts = {"contextual_profiles/use_global_layout"_i18n};
    for (const KeyMappingLayout& layout : *Settings::instance().get_mapping_laouts())
        layouts.push_back(layout.title);
    mappingLayout->init("settings/keys_mapping_title"_i18n, layouts, 0,
                        [this](int selected) {
                            if (!refreshing && selected >= 0 &&
                                static_cast<size_t>(selected) <=
                                    Settings::instance().get_mapping_laouts()->size()) {
                                if (selected == 0)
                                    profile.mappingLayout.reset();
                                else
                                    profile.mappingLayout = selected - 1;
                                saveProfile();
                            }
                        });

    loadMode(DeviceMode::Handheld);
}

ContextualProfilesView::~ContextualProfilesView() { Settings::instance().save(); }

void ContextualProfilesView::loadMode(DeviceMode nextMode) {
    mode = nextMode;
    profile = Settings::instance().device_profile(mode);
    refreshControls();
}

void ContextualProfilesView::saveProfile() {
    Settings::instance().set_device_profile(mode, profile, false);
    refreshModeSummaries();
}

void ContextualProfilesView::refreshControls() {
    refreshing = true;
    enabled->setOn(profile.enabled, false);
    bitrateOverride->setOn(profile.bitrate.has_value(), false);
    const int bitrate = profile.bitrate.value_or(Settings::instance().bitrate());
    bitrateHeader->setSubtitle(bitrate_text(bitrate));
    bitrateSlider->setProgress(std::clamp((bitrate - 500.0f) / 99500.0f,
                                          0.0f, 1.0f));

    int codecSelection = 0;
    if (profile.videoCodec) {
        const auto codecIt = std::find(codecs.begin(), codecs.end(),
                                       *profile.videoCodec);
        if (codecIt != codecs.end())
            codecSelection = static_cast<int>(codecIt - codecs.begin()) + 1;
    }
    codec->setSelection(codecSelection, true);

    int layoutSelection = 0;
    if (profile.mappingLayout &&
        Settings::instance().has_mapping_layout(*profile.mappingLayout))
        layoutSelection = *profile.mappingLayout + 1;
    mappingLayout->setSelection(layoutSelection, true);
    refreshing = false;

    const Visibility profileVisibility = profile.enabled ? Visibility::VISIBLE
                                                         : Visibility::GONE;
    bitrateOverride->setVisibility(profileVisibility);
    bitrateHeader->setVisibility(profile.enabled && profile.bitrate
                                     ? Visibility::VISIBLE
                                     : Visibility::GONE);
    bitrateSlider->setVisibility(profile.enabled && profile.bitrate
                                     ? Visibility::VISIBLE
                                     : Visibility::GONE);
    codec->setVisibility(profileVisibility);
    mappingLayout->setVisibility(profileVisibility);

    summary->setTitle(mode == DeviceMode::Handheld
                          ? "contextual_profiles/handheld"_i18n
                          : "contextual_profiles/docked"_i18n);
    summary->setSubtitle(profile_state(profile));
    refreshModeSummaries();
}

void ContextualProfilesView::refreshModeSummaries() {
    handheld->setDetailText(profile_state(
        Settings::instance().device_profile(DeviceMode::Handheld)));
    docked->setDetailText(profile_state(
        Settings::instance().device_profile(DeviceMode::Docked)));
}
