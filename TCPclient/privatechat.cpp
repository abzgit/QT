#include "privatechat.h"
#include "ui_privatechat.h"
#include <QMessageBox>

privatechat::privatechat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::privatechat)
{
    ui->setupUi(this);
}

privatechat::~privatechat()
{
    delete ui;
}

void privatechat::setchatname(QString strname)
{
    m_chatname = strname;
    m_strlogname = Tcpclient::getinstance().getlogname();
}

privatechat &privatechat::getinsance()
{
    static privatechat instance;
    return instance;
}

void privatechat::on_sendmsg_clicked()
{
    QString msg = ui->inputmsg->text();
    if(!msg.isEmpty()){
        pdu *Pdu = mkpud(msg.size()+1);
        Pdu->uiMsgType = msg_privatechat_request;
        memcpy(Pdu->caData,m_strlogname.toStdString().c_str(),m_strlogname.size());
        memcpy(Pdu->caData+32,m_chatname.toStdString().c_str(),m_chatname.size());
        strcpy((char*)(Pdu->caMsg),msg.toStdString().c_str());
        Tcpclient::getinstance().gettcpsocket().write((char*)Pdu,Pdu->uiPDUlen);
        free(Pdu);
        Pdu = NULL;

    }else{
        QMessageBox::warning(this,"私聊","发送的聊天信息不能为空!");
    }
}

