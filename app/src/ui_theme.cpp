#include "ui_theme.hpp"

#include <borealis.hpp>

namespace ApolloUI {

void applyTheme() {
    auto& dark = brls::Theme::getDarkTheme();
    auto& light = brls::Theme::getLightTheme();

    // Apollo surfaces are intentionally flat and inexpensive to render. Focus
    // is provided by Borealis' border highlight rather than a blurred backdrop.
    dark.addColor("apollo/canvas", nvgRGB(13, 17, 25));
    dark.addColor("apollo/surface", nvgRGB(23, 30, 43));
    dark.addColor("apollo/surface_elevated", nvgRGB(31, 41, 58));
    dark.addColor("apollo/accent", nvgRGB(71, 211, 224));
    dark.addColor("apollo/secondary", nvgRGB(163, 180, 199));
    dark.addColor("apollo/online", nvgRGB(74, 222, 128));
    dark.addColor("apollo/offline", nvgRGB(251, 146, 60));
    dark.addColor("apollo/error", nvgRGB(248, 113, 113));
    dark.addColor("brls/background", nvgRGB(13, 17, 25));
    dark.addColor("brls/sidebar/background", nvgRGB(18, 24, 35));
    dark.addColor("brls/sidebar/active_item", nvgRGB(71, 211, 224));
    dark.addColor("brls/sidebar/separator", nvgRGB(42, 53, 70));
    dark.addColor("brls/accent", nvgRGB(71, 211, 224));
    dark.addColor("brls/highlight/color1", nvgRGB(71, 211, 224));
    dark.addColor("brls/highlight/color2", nvgRGB(129, 235, 244));
    dark.addColor("brls/header/rectangle", nvgRGB(71, 211, 224));
    dark.addColor("brls/header/border", nvgRGB(42, 53, 70));
    dark.addColor("brls/header/subtitle", nvgRGB(163, 180, 199));
    dark.addColor("brls/list/listItem_value_color", nvgRGB(71, 211, 224));
    dark.addColor("brls/slider/line_filled", nvgRGB(71, 211, 224));

    light.addColor("apollo/canvas", nvgRGB(238, 243, 248));
    light.addColor("apollo/surface", nvgRGB(255, 255, 255));
    light.addColor("apollo/surface_elevated", nvgRGB(230, 239, 247));
    light.addColor("apollo/accent", nvgRGB(0, 132, 155));
    light.addColor("apollo/secondary", nvgRGB(76, 91, 110));
    light.addColor("apollo/online", nvgRGB(22, 163, 74));
    light.addColor("apollo/offline", nvgRGB(194, 65, 12));
    light.addColor("apollo/error", nvgRGB(220, 38, 38));
    light.addColor("brls/background", nvgRGB(238, 243, 248));
    light.addColor("brls/sidebar/background", nvgRGB(248, 251, 254));
    light.addColor("brls/sidebar/active_item", nvgRGB(0, 132, 155));
    light.addColor("brls/accent", nvgRGB(0, 132, 155));
    light.addColor("brls/highlight/color1", nvgRGB(0, 132, 155));
    light.addColor("brls/highlight/color2", nvgRGB(71, 211, 224));
    light.addColor("brls/header/rectangle", nvgRGB(0, 132, 155));
    light.addColor("brls/header/border", nvgRGB(202, 214, 225));
    light.addColor("brls/header/subtitle", nvgRGB(76, 91, 110));
    light.addColor("brls/list/listItem_value_color", nvgRGB(0, 132, 155));
    light.addColor("brls/slider/line_filled", nvgRGB(0, 132, 155));

    auto style = brls::getStyle();
    style.addMetric("apollo/space_xs", 8);
    style.addMetric("apollo/space_s", 12);
    style.addMetric("apollo/space_m", 20);
    style.addMetric("apollo/space_l", 32);
    style.addMetric("apollo/space_xl", 48);
    style.addMetric("apollo/card_radius", 16);
    style.addMetric("apollo/focus_padding", 4);
}

} // namespace ApolloUI
