#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QList>
#include "mytcpsocket.h"


class mytcpserver : public QTcpServer
{
    Q_OBJECT
public:
    explicit mytcpserver(QObject *parent = nullptr);
    static mytcpserver &getinstace();
    void incomingConnection(qintptr socketDescriptor);
    void transend(const char* pname,pdu *Pdu);   //广发
private:
    QList<MyTcpSocket*> m_socklist;
public slots:
    void deletesocket(MyTcpSocket * mysocket);
};

#endif // MYTCPSERVER_H
