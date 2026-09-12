//
//  host_tab.cpp
//  Moonlight
//
//  Created by XITRIX on 26.05.2021.
//

#include "host_tab.hpp"
#include "GameStreamClient.hpp"
#include "app_list_view.hpp"
#include "helper.hpp"
#include "main_tabs_view.hpp"
#include "stream_profiles_view.hpp"
#include "StreamProfileResolver.hpp"

#ifdef PLATFORM_SWITCH
#include "NetworkState.hpp"
#include "OperationMode.hpp"
#endif

using namespace brls::literals;

namespace {
std::string host_subtitle(const Host& host) {
    std::string subtitle = host.address;
    if (!host.remoteAddress.empty() && host.remoteAddress != host.address) {
        subtitle = subtitle.empty() ? host.remoteAddress
                                    : subtitle + " | " + host.remoteAddress;
    }
    return subtitle;
}
}

HostTab::HostTab(const Host& host) : host(host) {
    // Inflate the tab from the XML file
    this->inflateFromXMLRes("xml/tabs/host.xml");

    remove->setText("common/remove"_i18n);
    remove->title->setTextColor(RGB(229, 57, 53));

    reloadHost();

#ifdef PLATFORM_SWITCH
    streamProfiles->setText("stream_profiles/title"_i18n);
    updateStreamProfileSummary();
    streamProfiles->registerClickAction([this](View*) {
        present(new StreamProfilesView(this->host));
        return true;
    });
#else
    streamProfiles->removeFromSuperView(true);
#endif

    registerAction("host/rename"_i18n, ControllerButton::BUTTON_START,
                   [this](View* view) {
                       std::string title = this->host.hostname;
                       Application::getPlatform()->getImeManager()->openForText(
                               [this](const std::string& text) {
                                   this->host.hostname = text;
                                   Settings::instance().add_host(this->host);
                                   MainTabs::getInstanse()->refillTabs();
                               },
                               "host/rename_title"_i18n, "", 60, title, 0);

                       return true;
                   });

    connect->registerClickAction([this](View* view) {
        switch (state) {
        case AVAILABLE:
            this->present(new AppListView(this->host));
            break;
        case UNAVAILABLE:
            if (GameStreamClient::can_wake_up_host(this->host)) {
                const auto wakeRequestId = ++this->wakeRequestGeneration;
                this->canceledWakeRequestGeneration = 0;

                Dialog* loader =
                    createLoadingDialog("host/wake_up_message"_i18n,
                                        [this, wakeRequestId] {
                                            if (this->wakeRequestGeneration ==
                                                wakeRequestId) {
                                                this->canceledWakeRequestGeneration =
                                                    wakeRequestId;
                                            }
                                        });
                loader->open();

                ASYNC_RETAIN
                GameStreamClient::wake_up_host(
                    this->host,
                    [ASYNC_TOKEN, loader, wakeRequestId](
                        const GSResult<bool>& result) {
                        ASYNC_RELEASE

                        if (wakeRequestId != this->wakeRequestGeneration) {
                            return;
                        }

                        if (this->canceledWakeRequestGeneration ==
                            wakeRequestId) {
                            return;
                        }

                        loader->close([this, result, wakeRequestId] {
                            if (wakeRequestId != this->wakeRequestGeneration) {
                                return;
                            }

                            if (result.isSuccess()) {
                                reloadHost();
                            } else {
                                showError("host/wake_up_error"_i18n);
                            }
                        });
                    });
            }
            break;
        case FETCHING:
            break;
        }
        return true;
    });

    remove->registerClickAction([host](View* view) {
        auto* dialog = new Dialog("host/remove_message"_i18n);
        dialog->addButton("common/cancel"_i18n, [] {});
        dialog->addButton("common/remove"_i18n, [host] {
            Settings::instance().remove_host(host);
            MainTabs::getInstanse()->refillTabs();
        });
        dialog->open();

        return true;
    });
}

void HostTab::onFocusGained() {
    Box::onFocusGained();
#ifdef PLATFORM_SWITCH
    host = Settings::instance().host(host).value_or(host);
    updateStreamProfileSummary();
#endif
}

void HostTab::updateStreamProfileSummary() {
#ifdef PLATFORM_SWITCH
    const auto activeAddress = GameStreamClient::instance().active_address(host);
    const auto effective = StreamProfileResolver::resolve(
        host, activeAddress, NetworkState::current(), OperationMode::current());
    std::string detail = StreamProfileResolver::summary(effective) + "  •  " +
                         getVideoCodecName(effective.videoCodec);
    if (effective.context) {
        detail = "ACTIVE  •  " +
                 StreamProfileResolver::contextName(*effective.context) +
                 "  •  " + detail;
    } else {
        detail = "GLOBAL DEFAULTS  •  " + detail;
    }
    streamProfiles->setDetailText(detail);
    streamProfiles->setDetailTextColor(Application::getTheme()["apollo/secondary"]);
#endif
}

void HostTab::reloadHost() {
    host = Settings::instance().host(host).value_or(host);
    state = FETCHING;
    header->setTitle(host.hostname.empty() ? host.preferred_address()
                                           : host.hostname);
    header->setSubtitle("CHECKING CONNECTION  •  " + host_subtitle(host));
    connect->setText("Open library");
    connect->setDetailText("Checking host status");
    connect->setDetailTextColor(Application::getTheme()["apollo/secondary"]);

    ASYNC_RETAIN
    GameStreamClient::instance().connect(
        host, [ASYNC_TOKEN](const GSResult<SERVER_DATA>& result) {
            ASYNC_RELEASE

            if (result.isSuccess()) {
                const auto connectedAddress =
                    GameStreamClient::instance().active_address(this->host);
                const auto caps = ApolloCapabilities::detect(result.value());
                header->setSubtitle(
                    std::string("ONLINE") + (caps.isApollo ? "  •  APOLLO" : "") +
                    "  •  " + (connectedAddress.empty()
                                     ? host_subtitle(this->host)
                                     : connectedAddress));
                connect->setText("Open library");
                connect->setDetailText("Applications and games");
                connect->setDetailTextColor(
                    Application::getTheme()["apollo/online"]);
                state = AVAILABLE;

                this->host.apolloCaps = caps;
                Settings::instance().set_host_capabilities(this->host, caps);
                updateStreamProfileSummary();
            } else {
                header->setSubtitle("OFFLINE  •  " + host_subtitle(this->host));
                connect->setText("host/wake_up"_i18n);
                connect->setDetailText("Try Wake on LAN");
                connect->setDetailTextColor(
                    Application::getTheme()["apollo/offline"]);
                state = UNAVAILABLE;
            }
        });
}
