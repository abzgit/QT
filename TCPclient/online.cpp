#include "online.h"
#include "ui_online.h"
#include<QDebug>
#include "tcpclient.h"

online::online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::online)
{
    ui->setupUi(this);
}

online::~online()
{
    delete ui;
}

void online::showuse(pdu *Pdu)
{
    if(Pdu == NULL){
        return;
    }
    qDebug()<<"had showuse last";
    char tmp[32];
    uint uisize = Pdu->uiMsgLen/32;
    ui->onlinefri->clear();
    for(uint i = 0; i < uisize; ++i){
        memcpy(tmp,(char*)(Pdu->caMsg)+i*32,32);
        ui->onlinefri->addItem(tmp);
        qDebug()<<tmp;
    }
}

void online::on_pushButton_clicked()
{
    QListWidgetItem *item = ui->onlinefri->currentItem();
    if(item == NULL){
        return;
    }
    QString friname = item->text();
    QString mname = Tcpclient::getinstance().getlogname();
    pdu *Pdu = mkpud(0);
    Pdu->uiMsgType = msg_type_addfri_request;
    memcpy(Pdu->caData,friname.toStdString().c_str(),friname.size());
    memcpy(Pdu->caData+32,mname.toStdString().c_str(),mname.size());
    Tcpclient::getinstance().gettcpsocket().write((char*)Pdu,Pdu->uiPDUlen);
    free(Pdu);
    Pdu = NULL;
}

