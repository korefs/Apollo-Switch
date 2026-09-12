//
//  helper.cpp
//  Moonlight
//
//  Created by Даниил Виноградов on 29.05.2021.
//

#include "helper.hpp"

#include <utility>

using namespace brls;

void showAlert(std::string message, const std::function<void(void)>& cb) {
    auto alert = new brls::Dialog(std::move(message));
    alert->addButton("common/close"_i18n, [cb] { cb(); });
    alert->setCancelable(false);
    alert->open();
}

void showError(const std::string& message, const std::function<void(void)>& cb) {
    // Keep protocol and diagnostic text available without making it the first
    // thing a player sees. The continuation callback still runs on Close, or
    // after the Details sheet is dismissed, matching the old one-dialog flow.
    auto* error = new Dialog("Couldn't complete that action.");
    error->addButton("common/close"_i18n, cb);
    error->addButton("Details", [message, cb] {
        showAlert(message, cb);
    });
    error->setCancelable(false);
    error->open();
}

brls::Dialog* createLoadingDialog(
    const std::string& text, const std::function<void(void)>& onCancel) {
    Style style = Application::getStyle();
    Box* holder = new Box(Axis::COLUMN);

    auto* label = new Label();
    label->setText(text);
    label->setFontSize(style["brls/dialog/fontSize"]);
    label->setHorizontalAlign(HorizontalAlign::CENTER);
    label->setMarginBottom(21);

    auto* spinner = new ProgressSpinner(ProgressSpinnerSize::LARGE);
    spinner->View::setSize(Size(92, 92));

    holder->addView(label);
    holder->addView(spinner);

    holder->setAlignItems(AlignItems::CENTER);
    holder->setJustifyContent(JustifyContent::CENTER);
    holder->setPadding(style["brls/dialog/paddingTopBottom"],
                       style["brls/dialog/paddingLeftRight"], 28,
                       style["brls/dialog/paddingLeftRight"]);

    auto* dialog = new Dialog(holder);
    if (onCancel) {
        dialog->addButton("common/cancel"_i18n, [onCancel] { onCancel(); });
    }
    dialog->setCancelable(false);
    dialog->setFocusable(!onCancel);
    dialog->setHideHighlight(!onCancel);
    return dialog;
}
