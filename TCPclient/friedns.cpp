#include "friedns.h"
#include "online.h"

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
}

void friends::showonline()
{
    if(m_online->isHidden()){
        m_online->show();
    }else{
        m_online->hide();
    }
}