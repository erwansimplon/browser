#include <QApplication>
#include "primaryWindow.h"

primaryWindow *mainWindow;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    mainWindow = new primaryWindow();
    mainWindow->show();
    return app.exec();
};
