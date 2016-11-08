#ifndef PWINDOW_H
#define PWINDOW_H
#include <QPushButton>
#include <QWidgetAction>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QtWebKit>
#include <QtGui>
#include "bookmark.h"

class QAction;
class QActionGroup;
class QLabel;
class QMenu;
class QProgressBar;
class QLineEdit;
class QWebView;

class primaryWindow : public QMainWindow {
    Q_OBJECT
    public:
        primaryWindow();

    protected:
        #ifndef QT_NO_CONTEXTMENU
            void contextMenuEvent(QContextMenuEvent *event) Q_DECL_OVERRIDE;
        #endif // QT_NO_CONTEXTMENU

    public slots:
        void newOnglet(QString url = "");

    private slots:
        void openPrefOnglet();
        void saveSettings();

        void pageLoad();
        void searchLoad();

        void bookmark();
        void history();

        void closeOnglet(int);
        void previous();
        void next();
        void update();
        void home();
        void stop();

        void titleChange(const QString & title);
        void urlChange(const QUrl & url);
        void titleNUrlChange(int index);

        void myLoadStart();
        void myLoadProgress(int percent);
        void myLoadEnd(bool ok);

    private:
        // Config
        void loadConfig();
        QString settingsFile;
        QString defaultUrl;
        QString dataToSave;
        void saveBookmark(QString url);
        void loadBookmarks();
        void saveHistory(QString url);
        void loadHistory();
        QStringList listeHistorique;

        QWidget *prefWidget;
        QGridLayout *prefGrid;
        QLineEdit *defaultUrlSetting;
        QLabel *labelDefaultUrlSetting;
        QToolButton *saveDefaultUrlSetting;

        // Ui Creation
        void createActions();
        void createMenus();
        void CreateToolBar();
        void CreateStateBar();

        QWidget *ongletCreation(QString url = "");
        QWebView *actualPage();

        QTabWidget *onglets;
        QToolButton *buttonNewOnglet;

        QMenu *fileMenu;
        QAction *prefAction;
        QAction *quitAction;

        QMenu *navMenu;
        QAction *previousAction;
        QAction *nextAction;
        QAction *updateAction;
        QAction *homeAction;
        QAction *goUrlAction;
        QAction *goSearchAction;
        QAction *stopAction;
        QAction *bookmarkAction;
        QAction *historyAction;

        QLineEdit *urlBar;
        QLineEdit *searchBar;
        QProgressBar *progressBar;

        QToolBar *bookmarkToolBar;
        QToolBar *historyToolBar;
};
#endif
