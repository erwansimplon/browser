#ifndef BOOK_H
#define BOOK_H
#include <QToolButton>
#include "primaryWindow.h"

class bookmarkQToolButton : public QToolButton {
    Q_OBJECT
    public:
        bookmarkQToolButton();
        void setUrl(QString givenUrl);
        QString getUrl();

    public slots:
        void whenClicked();

    private:
        QString name;
        QString url;
};
#endif
