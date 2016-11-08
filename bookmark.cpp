#include <QToolButton>
#include "bookmark.h"
#include "main.cpp"

bookmarkQToolButton::bookmarkQToolButton() {
};

void bookmarkQToolButton::setUrl(QString givenUrl) {
    url = givenUrl;
};

QString bookmarkQToolButton::getUrl() {
    return url;
};

void bookmarkQToolButton::whenClicked() {
    mainWindow->newOnglet(url);
};
