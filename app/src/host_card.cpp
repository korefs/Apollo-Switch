#include "host_card.hpp"

#include "GameStreamClient.hpp"
#include "host_tab.hpp"
#include "integrations/apollo/ApolloCapabilities.hpp"
#include "main_tabs_view.hpp"

using namespace brls;

HostCard::HostCard(const Host& host) : host(host) {
    inflateFromXMLRes("xml/cells/host_card.xml");

    title->setText(host.hostname.empty() ? host.preferred_address() : host.hostname);
    connection->setText(host.remoteAddress.empty() ? "Saved host" : "Remote host");
    status->setText("CHECK STATUS");
    status->setTextColor(Application::getTheme()["apollo/secondary"]);

    setHideHighlightBackground(true);
    setHighlightPadding(brls::getStyle()["apollo/focus_padding"]);
    setHighlightCornerRadius(brls::getStyle()["apollo/card_radius"] +
                             brls::getStyle()["apollo/focus_padding"]);
    addGestureRecognizer(new TapGestureRecognizer(this));
    registerClickAction([this](View*) {
        openHost();
        return true;
    });
    registerAction("Quick actions", ControllerButton::BUTTON_X,
                   [this](View*) {
                       auto* dialog = new Dialog(this->host.hostname.empty()
                                                     ? this->host.preferred_address()
                                                     : this->host.hostname);
                       dialog->addButton("Open", [this] { openHost(); });
                       dialog->addButton("Rename", [this] {
                           const std::string currentName = this->host.hostname;
                           Application::getPlatform()->getImeManager()->openForText(
                               [this](const std::string& text) {
                                   this->host.hostname = text;
                                   Settings::instance().add_host(this->host);
                                   MainTabs::getInstanse()->refillTabs();
                               },
                               "Rename host", "", 60, currentName, 0);
                       });
                       dialog->addButton("Remove", [this] {
                           auto* confirmation =
                               new Dialog("Remove this host from Apollo Switch?");
                           confirmation->addButton("Cancel", [] {});
                           confirmation->addButton("Remove", [this] {
                               Settings::instance().remove_host(this->host);
                               MainTabs::getInstanse()->refillTabs();
                           });
                           confirmation->open();
                       });
                       dialog->open();
                       return true;
                   });
}

void HostCard::onFocusGained() {
    Box::onFocusGained();
    refreshStatus();
}

void HostCard::refreshStatus() {
    const auto generation = ++requestGeneration;
    status->setText("CHECKING");
    status->setTextColor(Application::getTheme()["apollo/secondary"]);

    ASYNC_RETAIN
    GameStreamClient::instance().connect(
        host, [ASYNC_TOKEN, generation](const GSResult<SERVER_DATA>& result) {
            ASYNC_RELEASE
            if (generation != requestGeneration)
                return;

            if (!result.isSuccess()) {
                status->setText("OFFLINE");
                status->setTextColor(Application::getTheme()["apollo/offline"]);
                return;
            }

            const auto caps = ApolloCapabilities::detect(result.value());
            Settings::instance().set_host_capabilities(host, caps);
            status->setText(caps.isApollo ? "ONLINE  •  APOLLO" : "ONLINE");
            status->setTextColor(Application::getTheme()["apollo/online"]);
        });
}

void HostCard::openHost() {
    auto* frame = new AppletFrame(new HostTab(host));
    frame->setTitle(host.hostname.empty() ? host.preferred_address() : host.hostname);
    Application::pushActivity(new Activity(frame));
}
