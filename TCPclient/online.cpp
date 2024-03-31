#include "online.h"
#include "ui_online.h"
#include<QDebug>

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
    for(uint i = 0; i < uisize; ++i){
        memcpy(tmp,(char*)(Pdu->caMsg)+i*32,32);
        ui->onlinefri->addItem(tmp);
        qDebug()<<"add ok!";
        qDebug()<<tmp;
    }
}
