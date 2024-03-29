#include "mytcpserver.h"

mytcpserver::mytcpserver(QObject *parent)
    : QTcpServer{parent}
{

}

mytcpserver &mytcpserver::getinstace()
{
    static mytcpserver instance;
    return instance;
}

void mytcpserver::incomingConnection(qintptr socketDescriptor)
{
    qDebug()<<"clientcomming";
    MyTcpSocket *tcpsock = new MyTcpSocket;
    tcpsock->setSocketDescriptor(socketDescriptor);
    m_socklist.append(tcpsock);
    connect(tcpsock,SIGNAL(offline(MyTcpSocket*)),this,SLOT(deletesocket(MyTcpSocket*)));
}

void mytcpserver::deletesocket(MyTcpSocket *mysocket)
{
    QList<MyTcpSocket*>::Iterator ite = m_socklist.begin();
    for(;ite != m_socklist.end();ite++){
        if(mysocket == *ite){
            delete *ite;
            *ite = NULL;
            m_socklist.erase(ite);
            break;
        }
    }
    for(int i = 0; i < m_socklist.size();++i){
        qDebug()<<m_socklist.at(i)->getname();
    }
}
