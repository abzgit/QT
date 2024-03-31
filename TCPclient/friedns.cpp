#include "friedns.h"
#include "online.h"
#include "tcpclient.h"
#include <QInputDialog>

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
}

void friends::showonlineuse(pdu *Pdu)
{
    if(Pdu == NULL){
        return;
    }
    qDebug()<<"had showuse";
    m_online->showuse(Pdu);
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
        qDebug()<<m_searchname;
        pdu *Pdu = mkpud(0);
        memcpy(Pdu->caData,m_searchname.toStdString().c_str(),m_searchname.size());
        Pdu->uiMsgType = msg_type_search_request;
        Tcpclient::getinstance().gettcpsocket().write((char*)Pdu,Pdu->uiPDUlen);
        free(Pdu);
        Pdu = NULL;
    }
}


