#pragma once
#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include "protocol.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class Tcpclient;
}
QT_END_NAMESPACE

class Tcpclient : public QWidget
{
    Q_OBJECT

public:
    Tcpclient(QWidget *parent = nullptr);
    ~Tcpclient();
    void loadconfig();
    static Tcpclient& getinstance();
    QTcpSocket& gettcpsocket();
    QString getlogname();
    void handregist(pdu *Pdu);
    void handlogin(pdu *Pdu);
    void handsearch(pdu *Pdu);
    void handaddrequest(pdu *Pdu);

private:
    Ui::Tcpclient *ui;
    QString m_ip;
    qint16 m_port;
    QTcpSocket m_sock;
    QString logname;

public slots:
    void showconnect();
    void recvmag();
private slots:
    // void on_pushButton_clicked();
    void on_login_clicked();
    void on_regist_clicked();
    void on_cancel_clicked();
};
#endif // TCPCLIENT_H
