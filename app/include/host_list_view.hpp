#pragma once

#include <borealis.hpp>

class HostListView : public brls::Box {
  public:
    HostListView();
    void willAppear(bool resetState) override;
    static brls::View* create();

  private:
    BRLS_BIND(brls::Box, container, "container");
    void reloadHosts();
};
