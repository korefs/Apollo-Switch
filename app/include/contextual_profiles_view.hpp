#pragma once

#include "DeviceProfile.hpp"
#include "Settings.hpp"
#include <borealis.hpp>
#include <vector>

class ContextualProfilesView : public brls::Box {
  public:
    ContextualProfilesView();
    ~ContextualProfilesView() override;

    BRLS_BIND(brls::DetailCell, handheld, "handheld");
    BRLS_BIND(brls::DetailCell, docked, "docked");
    BRLS_BIND(brls::Header, summary, "summary");
    BRLS_BIND(brls::BooleanCell, enabled, "enabled");
    BRLS_BIND(brls::BooleanCell, bitrateOverride, "bitrate_override");
    BRLS_BIND(brls::Header, bitrateHeader, "bitrate_header");
    BRLS_BIND(brls::Slider, bitrateSlider, "bitrate_slider");
    BRLS_BIND(brls::SelectorCell, codec, "codec");
    BRLS_BIND(brls::SelectorCell, mappingLayout, "mapping_layout");

  private:
    DeviceMode mode = DeviceMode::Handheld;
    ContextualDeviceProfile profile;
    bool refreshing = false;
    std::vector<VideoCodec> codecs;

    void loadMode(DeviceMode nextMode);
    void saveProfile();
    void refreshControls();
    void refreshModeSummaries();
};
