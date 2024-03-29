#include "mytcpsocket.h"
#include<QDebug>
#include "opdb.h"
#include <QMessageBox>

MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}
{
    connect(this,SIGNAL(readyRead()),this,SLOT(recvmsg()));
    connect(this,SIGNAL(disconnected()),this,SLOT(clientoffline()));
}

QString MyTcpSocket::getname()
{
    return m_name;
}

void MyTcpSocket::recvmsg()
{
    qDebug()<<this->bytesAvailable();
    uint uiPduLen = 0;
    this->read((char*)&uiPduLen,sizeof(uint));
    uint uiMsgLen = uiPduLen - sizeof(pdu);
    pdu *Pdu = mkpud(uiMsgLen);
    this->read((char*)Pdu + sizeof(uint),uiPduLen - sizeof(uint));
    switch (Pdu->uiMsgType) {
    case msg_type_regist_request:
    {
        char name[32] = {'\0'};
        char pwd[32] = {'\0'};
        strncpy(name,Pdu->caData,32);
        strncpy(pwd,Pdu->caData+32,32);
        bool ret = opDB::getinstance().handregist(name,pwd);
        pdu *respon= mkpud(0);
        respon->uiMsgType = msg_type_regist_respond;
        if(ret){
            strcpy(respon->caData,"regist ok!");
        }else{
            strcpy(respon->caData,"regist failed!name exited!");
        }
        write((char*)respon,respon->uiPDUlen);
        free(respon);
        respon = NULL;
        break;
    }
    case msg_type_login_request:
    {
        char name[32] = {'\0'};
        char pwd[32] = {'\0'};
        strncpy(name,Pdu->caData,32);
        strncpy(pwd,Pdu->caData+32,32);
        bool ret = opDB::getinstance().handlogin(name,pwd);
        pdu *respon= mkpud(0);
        respon->uiMsgType = msg_type_login_respond;
        if(ret){
            strcpy(respon->caData,"login ok!");
            m_name = name;
        }else{
            strcpy(respon->caData,"no account!");
        }
        write((char*)respon,respon->uiPDUlen);
        free(respon);
        respon = NULL;
        break;
    }
    default:
        break;
    }
    // qDebug()<<name<<pwd;
}

void MyTcpSocket::clientoffline()
{
    opDB::getinstance().handoffline(m_name.toStdString().c_str());
    emit offline(this);

}
