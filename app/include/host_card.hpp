#pragma once

#include "Settings.hpp"
#include <borealis.hpp>

class HostCard : public brls::Box {
  public:
    explicit HostCard(const Host& host);

    void onFocusGained() override;

  private:
    Host host;
    uint64_t requestGeneration = 0;

    BRLS_BIND(brls::Label, title, "title");
    BRLS_BIND(brls::Label, connection, "connection");
    BRLS_BIND(brls::Label, status, "status");

    void refreshStatus();
    void openHost();
};
