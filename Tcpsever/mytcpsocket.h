#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "protocol.h"

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
signals:
    void offline(MyTcpSocket * mysocket);
public:
    explicit MyTcpSocket(QObject *parent = nullptr);
    QString getname();
public slots:
    void recvmsg();
    void clientoffline();
private:
    QString m_name;
};

#endif // MYTCPSOCKET_H
