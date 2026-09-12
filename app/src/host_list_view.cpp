#include "host_list_view.hpp"

#include "Settings.hpp"
#include "add_host_tab.hpp"
#include "grid_view.hpp"
#include "host_card.hpp"

using namespace brls;
using namespace brls::literals;

HostListView::HostListView() {
    inflateFromXMLRes("xml/tabs/hosts.xml");
    reloadHosts();
}

void HostListView::willAppear(bool resetState) {
    Box::willAppear(resetState);
    reloadHosts();
}

void HostListView::reloadHosts() {
    container->clearViews();

    auto* heading = new Header();
    heading->setTitle("Your PCs");
    heading->setSubtitle("Choose a host to play");
    heading->setLineBottom(0);
    container->addView(heading);

    auto* hosts = new GridView(2);
    for (const Host& host : Settings::instance().hosts()) {
        hosts->addView(new HostCard(host));
    }
    container->addView(hosts);

    auto* addHost = new DetailCell();
    addHost->setText("Add host");
    addHost->setDetailText("Pair a gaming PC");
    addHost->setDetailTextColor(Application::getTheme()["apollo/secondary"]);
    addHost->setMarginTop(brls::getStyle()["apollo/space_m"]);
    addHost->registerClickAction([](View*) {
        Application::pushActivity(new Activity(new AppletFrame(new AddHostTab())));
        return true;
    });
    container->addView(addHost);
}

View* HostListView::create() { return new HostListView(); }
