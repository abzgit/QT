#include "tcpsever.h"
#include "ui_tcpsever.h"
#include "mytcpserver.h"
#include <QFile>
#include <QDebug>

TcpSever::TcpSever(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpSever)
{
    ui->setupUi(this);
    loadconfig();
    mytcpserver::getinstace().listen(QHostAddress(m_ip),m_port);
}

TcpSever::~TcpSever()
{
    delete ui;
}

void TcpSever::loadconfig()
{
    QFile file(":/clientconfig.txt");
    if(file.open(QIODevice::ReadOnly)){
        QByteArray badata = file.readAll();
        QString strdata = badata.toStdString().c_str();
        strdata.replace("\r\n"," ");
        QStringList datas = strdata.split(" ");
        m_ip = datas.at(0);
        m_port = datas.at(1).toUShort();
        file.close();
    }else{
        qDebug()<<"error";
    }
}
