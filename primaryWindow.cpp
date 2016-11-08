#include <QApplication>
#include <QGridLayout>
#include <QtWidgets>
#include <QWidgetAction>
#include <QMenuBar>
#include <QProgressBar>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QWebView>
#include <QtWebKit>
#include <QSettings>
#include "primaryWindow.h"

primaryWindow::primaryWindow() {
    loadConfig();

    createActions();
    createMenus();
    CreateStateBar();
    CreateToolBar();

    onglets = new QTabWidget;
    onglets->setTabsClosable(true);
    onglets->setMovable(true);

    onglets->addTab(new QLabel("Add tabs by pressing \"+\""), QString());
    onglets->setTabEnabled(0, false);

    QToolButton *buttonNewOnglet = new QToolButton(this);
    buttonNewOnglet->setIcon(QIcon("img/plus.png"));
    buttonNewOnglet->setFixedSize(20, 20);
    buttonNewOnglet->setStyleSheet(QString("border: 0px;"));
    connect(buttonNewOnglet, SIGNAL(clicked()), this, SLOT(newOnglet()));

    onglets->tabBar()->setTabButton(0, QTabBar::RightSide, buttonNewOnglet);

    connect(onglets, SIGNAL(tabCloseRequested(int)), this, SLOT(closeOnglet(int)));
    connect(onglets, SIGNAL(currentChanged(int)), this, SLOT(titleNUrlChange(int)));

    newOnglet();

    setCentralWidget(onglets);

    setWindowTitle("FNavigateur");
    resize(1200, 600);
};

/*-----------------------------------------------------------------*/

#ifndef QT_NO_CONTEXTMENU
void primaryWindow::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);
    menu.exec(event->globalPos());
};
#endif

/*-----------------------------------------------------------------*/

void primaryWindow::loadConfig() {
    settingsFile = "config.ini";
    QSettings settings(settingsFile, QSettings::IniFormat);
    defaultUrl = settings.value("urlDefaultPage", "").toString();
};

void primaryWindow::saveSettings() {
    QSettings settings(settingsFile, QSettings::IniFormat);
    QString dataToSave = defaultUrlSetting->text();
    settings.setValue("urlDefaultPage", dataToSave);
    loadConfig();
};

void primaryWindow::loadBookmarks() {
    QSettings settings(settingsFile, QSettings::IniFormat);
    settings.beginGroup("bookmarks");

    int size = settings.beginReadArray("bookmark");

    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QString url = settings.value("url").toString();

        bookmarkQToolButton *bookmarkButton = new bookmarkQToolButton();
        bookmarkButton->setUrl(url);
        QStringList modifiedUrl = url.split(".");
        bookmarkButton->setText(modifiedUrl.at(1));

        connect(bookmarkButton, SIGNAL(clicked()), bookmarkButton, SLOT(whenClicked()));

        bookmarkToolBar->addWidget(bookmarkButton);
    }
};

void primaryWindow::saveBookmark(QString url) {
    QSettings settings(settingsFile, QSettings::IniFormat);
    settings.beginGroup("bookmarks");

    int size = settings.beginReadArray("bookmark");
    settings.endArray();

    settings.beginWriteArray("bookmark");
    settings.setArrayIndex(size);

    settings.setValue("url", url);

    settings.endArray();
    settings.endGroup();
};

/*-----------------------------------------------------------------*/

void primaryWindow::createActions() {
    quitAction = new QAction(tr("E&xit"), this);
    quitAction->setShortcuts(QKeySequence::Quit);
    quitAction->setStatusTip(tr("Exit the application"));
    connect(quitAction, &QAction::triggered, this, &QWidget::close);
    /*-----------------------------------------------------------------*/
    prefAction = new QAction(tr("Préférence"), this);
    connect(prefAction, SIGNAL(triggered()), this, SLOT(openPrefOnglet()));
    /*-----------------------------------------------------------------*/
    goSearchAction = new QAction(QIcon("img/go.png"), "go", this);
    connect(goSearchAction, SIGNAL(triggered()), this, SLOT(searchLoad()));
    /*-----------------------------------------------------------------*/
    previousAction = new QAction(QIcon("img/prec.png"), "Précédent", this);
    connect(previousAction, SIGNAL(triggered()), this, SLOT(previous()));
    /*-----------------------------------------------------------------*/
    nextAction = new QAction(QIcon("img/suiv.png"), "Suivant", this);
    connect(nextAction, SIGNAL(triggered()), this, SLOT(next()));
    /*-----------------------------------------------------------------*/
    updateAction = new QAction(QIcon("img/actu.png"), "Actualiser", this);
    connect(updateAction, SIGNAL(triggered()), this, SLOT(update()));
    /*-----------------------------------------------------------------*/
    homeAction = new QAction(QIcon("img/home.png"), "Home", this);
    connect(homeAction, SIGNAL(triggered()), this, SLOT(home()));
    /*-----------------------------------------------------------------*/
    stopAction = new QAction(QIcon("img/stop.png"), "Stop", this);
    connect(stopAction, SIGNAL(triggered()), this, SLOT(stop()));
    /*-----------------------------------------------------------------*/
    bookmarkAction = new QAction(QIcon("img/bookm.png"), "Bookmark", this);
    connect(bookmarkAction, SIGNAL(triggered()), this, SLOT(bookmark()));

    historyAction = new QAction(QIcon("img/histo.png"), "history", this);
    connect(historyAction, SIGNAL(triggered()), this, SLOT(history()));
};

void primaryWindow::createMenus() {
    fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(prefAction);
    fileMenu->addAction(quitAction);

    navMenu = menuBar()->addMenu("&Navigation");
    navMenu->addAction(previousAction);
    navMenu->addAction(nextAction);
    navMenu->addAction(updateAction);
    navMenu->addAction(homeAction);
    navMenu->addAction(stopAction);
};

void primaryWindow::CreateToolBar() {
    urlBar = new QLineEdit();
    urlBar->setFixedWidth(700);
    connect(urlBar, SIGNAL(returnPressed()), this, SLOT(pageLoad()));

    searchBar = new QLineEdit();
    searchBar->setFixedWidth(220);
    connect(searchBar, SIGNAL(returnPressed()), this, SLOT(searchLoad()));

    QToolBar *navigationToolBar = addToolBar("Navigation");
    navigationToolBar->addAction(homeAction);
    navigationToolBar->addAction(previousAction);
    navigationToolBar->addAction(nextAction);
    navigationToolBar->addWidget(urlBar);
    navigationToolBar->addAction(updateAction);
    navigationToolBar->addAction(stopAction);
    navigationToolBar->addWidget(searchBar);
    navigationToolBar->addAction(goSearchAction);
    navigationToolBar->addAction(bookmarkAction);
    navigationToolBar->addAction(historyAction);

    addToolBarBreak();

    bookmarkToolBar = addToolBar("Bookmark");

    addToolBarBreak();

    historyToolBar = addToolBar("history");

    loadBookmarks();
    loadHistory();

};

void primaryWindow::CreateStateBar() {
    progressBar = new QProgressBar(this);
    progressBar->setVisible(false);
    progressBar->setMaximumHeight(12);
    statusBar()->addWidget(progressBar, 1);
};

/*-----------------------------------------------------------------*/

void primaryWindow::searchLoad() {
    QString url = searchBar->text();
    std::replace(url.begin(), url.end(), ' ', '+');
    urlBar->setText("http://www.google.fr/search?q=" + url);
    pageLoad();
};

void primaryWindow::pageLoad() {
    QString url = urlBar->text();

    if (url.left(7) != "http://") {
        url = "http://" + url;
        urlBar->setText(url);
    }

    actualPage()->load(QUrl(url));
};

/*-----------------------------------------------------------------*/

void primaryWindow::openPrefOnglet() {
    QWidget *prefWidget = new QWidget;
    QGridLayout *prefGrid = new QGridLayout;
    prefWidget->setLayout(prefGrid);

    // Default Site
    defaultUrlSetting = new QLineEdit();
    defaultUrlSetting->setFixedWidth(350);
    defaultUrlSetting->setText(defaultUrl);

    QLabel *labelDefaultUrlSetting = new QLabel();
    labelDefaultUrlSetting->setText("Page d'acceuil: ");

    QToolButton *saveDefaultUrlSetting = new QToolButton();
    saveDefaultUrlSetting->setText("Save");
    connect(saveDefaultUrlSetting, SIGNAL(clicked()), this, SLOT(saveSettings()));

    prefGrid->addWidget(labelDefaultUrlSetting, 0, 0);
    prefGrid->addWidget(defaultUrlSetting, 0, 1);
    prefGrid->addWidget(saveDefaultUrlSetting, 0, 2, 1, 8);

    // --------------------------------------------------------------

    onglets->insertTab(onglets->count() - 1, prefWidget, "Préférence");
    onglets->setCurrentIndex(onglets->count() - 2);
};

/*-----------------------------------------------------------------*/

void primaryWindow::bookmark() {
    bookmarkQToolButton *bookmarkButton = new bookmarkQToolButton();
    QString url = urlBar->text();

    if(url.left(8) == "https://") {
            url.replace("https://", "http://");
    }

    saveBookmark(url);

    bookmarkButton->setUrl(url);

    QStringList modifiedUrl = url.split(".");
    bookmarkButton->setText(modifiedUrl.at(1));
    bookmarkButton->setFixedSize(90, 20);
    //bookmarkButton->setStyleSheet(QString("border: 0px;"));
    connect(bookmarkButton, SIGNAL(clicked()), bookmarkButton, SLOT(whenClicked()));

    bookmarkToolBar->addWidget(bookmarkButton);
};

/*-----------------------------------------------------------------*/

void primaryWindow::newOnglet(QString url) {
    int indexNewOnglet = onglets->count() - 1;
    onglets->insertTab(indexNewOnglet, ongletCreation(url), "Nouvelle Page");
    onglets->setCurrentIndex(indexNewOnglet);

    urlBar->setText("");
    urlBar->setFocus(Qt::OtherFocusReason);
};

void primaryWindow::closeOnglet(int index) {
    if(onglets->count() > 2) {
        onglets->removeTab(index);
    }
};

void primaryWindow::previous() {
    actualPage()->back();
};

void primaryWindow::next() {
    actualPage()->forward();
};

void primaryWindow::update() {
    actualPage()->reload();
};

void primaryWindow::home() {
    actualPage()->load(QUrl(defaultUrl));
    urlBar->setText(defaultUrl);
};

void primaryWindow::stop() {
    actualPage()->stop();
};

/*-----------------------------------------------------------------*/

void primaryWindow::titleChange(const QString & title) {
    QString newTitle = title;

    if(title.size() > 20) {
        newTitle = title.left(20) + "...";
    }

    setWindowTitle(newTitle);
    onglets->setTabText(onglets->currentIndex(), newTitle);
};

void primaryWindow::urlChange(const QUrl & url) {
    urlBar->setText(url.toString());
};

void primaryWindow::titleNUrlChange(int index) {
    QString url = onglets->currentWidget()->findChild<QWebView *>()->url().toString();
    urlBar->setText(url);

    QString title = onglets->currentWidget()->findChild<QWebView *>()->title();

    if(title.size() > 20) {
        title = title.left(20) + "...";
    }

    setWindowTitle(title);
};

/*-----------------------------------------------------------------*/

void primaryWindow::myLoadStart() {
    progressBar->setVisible(true);
};

void primaryWindow::myLoadProgress(int percent) {
    progressBar->setValue(percent);
};

void primaryWindow::myLoadEnd(bool ok) {
    progressBar->setVisible(false);

};

/*-----------------------------------------------------------------*/

QWidget *primaryWindow::ongletCreation(QString url) {
    QWidget *ongletPage = new QWidget;
    QWebView *webPage = new QWebView;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(webPage);
    ongletPage->setLayout(layout);

    if(url == "") {
        webPage->load(QUrl(defaultUrl));
    } else {
        if(url.left(7) != "http://") {
            url = "http://" + url;
        }
        webPage->load(QUrl(url));
    }

    connect(webPage, SIGNAL(titleChanged(QString)), this, SLOT(titleChange(QString)));
    connect(webPage, SIGNAL(urlChanged(QUrl)), this, SLOT(urlChange(QUrl)));
    connect(webPage, SIGNAL(loadStarted()), this, SLOT(myLoadStart()));
    connect(webPage, SIGNAL(loadProgress(int)), this, SLOT(myLoadProgress(int)));
    connect(webPage, SIGNAL(loadFinished(bool)), this, SLOT(myLoadEnd(bool)));

    return ongletPage;
};

QWebView *primaryWindow::actualPage() {
    return onglets->currentWidget()->findChild<QWebView *>();
};

void primaryWindow::loadHistory() {
    QSettings settings(settingsFile, QSettings::IniFormat);
    settings.beginGroup("history");

    int size = settings.beginReadArray("history");

    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QString url = settings.value("url").toString();

        bookmarkQToolButton *historyButton = new bookmarkQToolButton();
        historyButton->setUrl(url);
        QStringList modifiedUrl = url.split(".");
        historyButton->setText(modifiedUrl.at(1));
        listeHistorique << url;

        //historyToolBar->addWidget(historyButton);
    }
};

void primaryWindow::saveHistory(QString url) {
    QSettings settings(settingsFile, QSettings::IniFormat);
    settings.beginGroup("history");

    int size = settings.beginReadArray("history");
    settings.endArray();

    settings.beginWriteArray("history");
    settings.setArrayIndex(size);

    settings.setValue("url", url);

    settings.endArray();
    settings.endGroup();
}

void primaryWindow::history() {
    QList<QWebHistoryItem> liste = actualPage()->history()->items(); // Ça c'est l'historique // Ça c'est la liste qu'on va remplir
    QString url = urlBar->text();

    for(int i = 0 ; i < liste.size() ; i++)
        // Pour chaque page de l'historique, on récupère son url converti en QString et on l'ajoute à la liste
        listeHistorique << liste[i].url().toString();
    loadHistory();
    QVBoxLayout *layout = new QVBoxLayout;

    // "listeHistorique" contient maintenant tous les url de l'historique, on peut l'utiliser pour le modèle
    QStringListModel *modele = new QStringListModel(listeHistorique);

    QListView *vue = new QListView;
    vue->setModel(modele);

    layout->addWidget(vue);

    QWebView *mainWindow = new QWebView;
    mainWindow->setLayout(layout);
    saveHistory(url);
    mainWindow->show();
}
