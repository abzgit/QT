#include "mytcpsocket.h"
#include<QDebug>
#include "opdb.h"
#include <QMessageBox>
#include "mytcpserver.h"

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
            strcpy(respon->caData,REGIST_OK);
        }else{
            strcpy(respon->caData,REGIST_FAILED);
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
            strcpy(respon->caData,LOGIN_OK);
            m_name = name;
        }else{
            strcpy(respon->caData,LOGIN_FAILED);
        }
        write((char*)respon,respon->uiPDUlen);
        free(respon);
        respon = NULL;
        break;
    }
    case msg_type_online_request:{
        QStringList ret = opDB::getinstance().handonline();
        uint len = ret.size()*32;
        pdu *Pdu1 = mkpud(len);
        Pdu1->uiMsgType = msg_type_online_respond;
        for(int i = 0; i < ret.size(); ++i){
            memcpy((char*)(Pdu1->caMsg)+i*32,ret[i].toStdString().c_str(),ret[i].size());
        }
        write((char*)Pdu1,Pdu1->uiPDUlen);
        free(Pdu1);
        Pdu1 = NULL;
        break;
    }
    case msg_type_search_request:{
        int ret = opDB::getinstance().handsearch(Pdu->caData);
        pdu *respon = mkpud(0);
        respon->uiMsgType = msg_type_search_respond;
        if(ret == -1){
            strcpy(respon->caData,SEARCH_USER_EMPTY);
        }else if(ret == 1){
            strcpy(respon->caData,SEARCH_USER_OK);
        }else{
            strcpy(respon->caData,SEARCH_USER_OFFLINE);
        }
        write((char*)respon,respon->uiPDUlen);
        free(respon);
        respon = NULL;
        break;
    }
    case msg_type_addfri_request:{
        char pname[32] = {'\0'};
        char mname[32] = {'\0'};
        strncpy(pname,Pdu->caData,32);
        strncpy(mname,Pdu->caData+32,32);
        int ret = opDB::getinstance().handadd(pname,mname);
        pdu *respon = mkpud(0);
        respon->uiMsgType = msg_type_addfri_respond;
        if(ret == -1){
            strcpy(respon->caData,UNKNOW_ERROR);
            write((char*)respon,respon->uiPDUlen);
            free(respon);
            respon = NULL;
        }else if(0 == ret){
            strcpy(respon->caData,EXEITED_FRIEND);
            write((char*)respon,respon->uiPDUlen);
            free(respon);
            respon = NULL;
        }else if(1 == ret){
            mytcpserver::getinstace().transend(pname,Pdu);
            write((char*)respon,respon->uiPDUlen);
            free(respon);
            respon = NULL;
        }else if(2 == ret){
            strcpy(respon->caData,ADD_FRIEND_OFFLINE);
            write((char*)respon,respon->uiPDUlen);
            free(respon);
            respon = NULL;
        }else if(3 == ret){
            strcpy(respon->caData,ADD_FRIEND_NO_EXIT);
            write((char*)respon,respon->uiPDUlen);
            free(respon);
            respon = NULL;
        }
        break;
    }
    case msg_type_agree_add_friend:{
        char addedName[32] = {'\0'};
        char sourceName[32] = {'\0'};
        // 拷贝读取的信息
        strncpy(addedName, Pdu -> caData, 32);
        strncpy(sourceName, Pdu -> caData + 32, 32);
        opDB::getinstance().handagreeadd(addedName,sourceName);
        mytcpserver::getinstace().transend(sourceName,Pdu);
    }
    case msg_typr_disagree_add:{
        char sourceName[32] = {'\0'};
        // 拷贝读取的信息
        strncpy(sourceName, Pdu -> caData + 32, 32);
        // 服务器需要转发给发送好友请求方其被拒绝的消息
        mytcpserver::getinstace().transend(sourceName,Pdu);
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
