#include "openwidget.h"

openwidget::openwidget(QWidget *parent)
    : QWidget{parent}
{
    mlistw = new QListWidget(this);
    mlistw->addItem("好友");
    mlistw->addItem("图书");

    m_friend = new friends;
    m_book = new book;
    m_staw = new QStackedWidget;

    m_staw->addWidget(m_friend);
    m_staw->addWidget(m_book);;

    QHBoxLayout *pmain = new QHBoxLayout;
    pmain->addWidget(mlistw);
    pmain->addWidget(m_staw);

    setLayout(pmain);

    connect(mlistw,SIGNAL(currentRowChanged(int)),m_staw,SLOT(setCurrentIndex(int)));
}

openwidget &openwidget::getinstance()
{
    static openwidget instance;
    return instance;
}

friends *openwidget::getfriend()
{
    return m_friend;
}
