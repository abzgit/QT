#include "friedns.h"
#include "online.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QStringList>
#include "privatechat.h"
#include <QMessageBox>

friends ::friends(QWidget *parent)
    : QWidget{parent}
{
    m_msgte = new QTextEdit; //显示信息
    m_friendlist = new QListWidget;  //好友列表
    m_inputmag = new QLineEdit;  //输入框

    m_delfriend = new QPushButton("删除好友");
    m_flushfriend = new QPushButton("刷新好友状态");
    m_onlineuser = new QPushButton("在线用户");
    m_searchuser = new QPushButton("搜索好友");
    m_sendmag = new QPushButton("发送");
    m_privatechat = new QPushButton("私聊");

    QVBoxLayout *rightPb = new QVBoxLayout;
    rightPb->addWidget(m_delfriend);
    rightPb->addWidget(m_flushfriend);
    rightPb->addWidget(m_onlineuser);
    rightPb->addWidget(m_searchuser);
    rightPb->addWidget(m_privatechat);

    QHBoxLayout *top = new QHBoxLayout;
    top->addWidget(m_msgte);
    top->addWidget(m_friendlist);
    top->addLayout(rightPb);

    QHBoxLayout *pmsg = new QHBoxLayout;
    pmsg->addWidget(m_inputmag);
    pmsg->addWidget(m_sendmag);

    m_online = new online;

    QVBoxLayout *pmain = new QVBoxLayout;
    pmain->addLayout(top);
    pmain->addLayout(pmsg);
    pmain->addWidget(m_online);
    m_online->hide();

    setLayout(pmain);
    connect(m_onlineuser,SIGNAL(clicked(bool)),this,SLOT(showonline()));
    connect(m_searchuser,SIGNAL(clicked(bool)),this,SLOT(searchuse()));
    connect(m_flushfriend,SIGNAL(clicked(bool)),this,SLOT(flushfriend()));
    connect(m_delfriend,SIGNAL(clicked(bool)),this,SLOT(delfriend()));
    connect(m_privatechat,SIGNAL(clicked(bool)),this,SLOT(privatechat()));
    connect(m_sendmag,SIGNAL(clicked(bool)),this,SLOT(groupchat()));
}

void friends::showonlineuse(pdu *Pdu)
{
    if(Pdu == NULL){
        return;
    }
    m_online->showuse(Pdu);
}

void friends::handflush(pdu *Pdu)
{
    if(Pdu == NULL){
        return;
    }
    uint datasize = Pdu->uiMsgLen/32;
    char name[32] = {'\0'};
    m_friendlist->clear();
    for(uint i = 0; i < datasize; ++i){
        memcpy(name,(char *)(Pdu->caMsg)+i*32,32);
        m_friendlist->addItem(name);
    }
}

void friends::handgroupchat(pdu *Pdu)
{

    QString strmsg = QString("%1 says: %2").arg(Pdu->caData).arg((char*)(Pdu->caMsg));

    m_msgte->append(strmsg);
}

void friends::showonline()
{
    if(m_online->isHidden()){
        m_online->show();
        pdu *Pdu = mkpud(0);
        Pdu->uiMsgType = msg_type_online_request;
        Tcpclient::getinstance().gettcpsocket().write((char*)Pdu,Pdu->uiPDUlen);
        free(Pdu);
        Pdu = NULL;
    }else{
        m_online->hide();
    }
}

void friends::searchuse()
{
    m_searchname = QInputDialog::getText(this,"搜索","用户名");
    if(!m_searchname.isEmpty()){
        pdu *Pdu = mkpud(0);
        memcpy(Pdu->caData,m_searchname.toStdString().c_str(),m_searchname.size());
        Pdu->uiMsgType = msg_type_search_request;
        Tcpclient::getinstance().gettcpsocket().write((char*)Pdu,Pdu->uiPDUlen);
        free(Pdu);
        Pdu = NULL;
    }
}

void friends::flushfriend()
{
    QString strname = Tcpclient::getinstance().getlogname();
    pdu *Pdu = mkpud(0);
    Pdu->uiMsgType = msg_flush_request;
    memcpy(Pdu->caData,strname.toStdString().c_str(),strname.size());
    Tcpclient::getinstance().gettcpsocket().write((char*)Pdu,Pdu->uiPDUlen);
    free(Pdu);
    Pdu = NULL;
}

void friends::delfriend()
{
    if(m_friendlist->currentItem() != NULL){
        QString friname = m_friendlist->currentItem()->text();
        QStringList strname = friname.split(" ");
        QString fname = strname[0];
        pdu *Pdu = mkpud(0);
        Pdu->uiMsgType = msg_delfriend_request;
        QString mname = Tcpclient::getinstance().getlogname();
        memcpy(Pdu->caData,mname.toStdString().c_str(),mname.size());
        memcpy(Pdu->caData+32,fname.toStdString().c_str(),fname.size());
        Tcpclient::getinstance().gettcpsocket().write((char*)Pdu,Pdu->uiPDUlen);
        free(Pdu);
        Pdu = NULL;
    }
}

void friends::privatechat()
{
    if(NULL != m_friendlist->currentItem()){
        QString chatname = m_friendlist->currentItem()->text();
        QStringList list = chatname.split(" ");
        chatname = list[0];
        privatechat::getinsance().setchatname(chatname);
        if(privatechat::getinsance().isHidden()){
            privatechat::getinsance().show();
        }
    }else{
        QMessageBox::warning(this,"私聊","请选择私聊的对象");
    }
}

void friends::groupchat()
{
    QString msg = m_inputmag->text();
    if(!msg.isEmpty()){
        pdu *Pdu = mkpud(msg.size()+1);
        Pdu->uiMsgType = msg_group_chat_request;
        QString strname = Tcpclient::getinstance().getlogname();
        strncpy(Pdu->caData,strname.toStdString().c_str(),strname.size());
        strncpy((char*)(Pdu->caMsg),msg.toStdString().c_str(),msg.size());
        Tcpclient::getinstance().gettcpsocket().write((char*)Pdu,Pdu->uiPDUlen);
    }else{
        QMessageBox::warning(this,"群聊","发送消息不能为空");
    }
}


