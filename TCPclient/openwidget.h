#ifndef OPENWIDGET_H
#define OPENWIDGET_H

#include <QWidget>
#include<QListWidget>
#include "friedns.h"
#include "book.h"
#include <QStackedWidget>

class openwidget : public QWidget
{
    Q_OBJECT
public:
    explicit openwidget(QWidget *parent = nullptr);

signals:

private:
    QListWidget *mlistw;
    friends *m_friend;
    book *m_book;
    QStackedWidget *m_staw;
};

#endif // OPENWIDGET_H
