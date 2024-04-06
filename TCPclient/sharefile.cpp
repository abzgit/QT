#include "sharefile.h"
#include "tcpclient.h"
#include "openwidget.h"

sharefile::sharefile(QWidget *parent)
    : QWidget{parent}
{
    allselect = new QPushButton("全选");
    allcancle = new QPushButton("取消选择");
    ok = new QPushButton("确定");
    cancle = new QPushButton("取消");

    m_PSA = new QScrollArea;
    m_pFriendW = new QWidget;
    mfriendwidget = new QVBoxLayout(m_pFriendW);
    mbuttongroup = new QButtonGroup(m_pFriendW);
    mbuttongroup->setExclusive(false);

    QHBoxLayout *top = new QHBoxLayout;
    top->addWidget(allselect);
    top->addWidget(allcancle);
    top->addStretch();

    QHBoxLayout *down = new QHBoxLayout;
    down->addWidget(ok);
    down->addWidget(cancle);

    QVBoxLayout *pmain = new QVBoxLayout;
    pmain->addLayout(top);
    pmain->addWidget(m_PSA);
    pmain->addLayout(down);
    setLayout(pmain);

    connect(cancle,SIGNAL(clicked(bool)),this,SLOT(cancleselect()));
    connect(allselect,SIGNAL(clicked(bool)),this,SLOT(selectall()));
    connect(ok,SIGNAL(clicked(bool)),this,SLOT(choseok()));
    connect(cancle,SIGNAL(clicked(bool)),this,SLOT(chosecancle()));

}

sharefile &sharefile::getinstance()
{
    static sharefile instance;
    return instance;
}

void sharefile::updatefriend(QListWidget *pfriendlist)
{
    if(NULL == pfriendlist){
        return;
    }
    QAbstractButton *tmp = NULL;
    QList<QAbstractButton*> prefriend = mbuttongroup->buttons();
    for(int i = 0; i < prefriend.count(); ++i){
        tmp = prefriend[i];
        mfriendwidget->removeWidget(tmp);
        mbuttongroup->removeButton(tmp);
        prefriend.removeOne(tmp);
        delete tmp;
        tmp = NULL;
    }
    QCheckBox *pcb = NULL;
    for(int i = 0;i < pfriendlist->count(); ++i){
        QStringList a = pfriendlist->item(i)->text().split(" ");
        if(a.last() == "offline"){
            continue;
        }
        pcb = new QCheckBox(a[0]);
        mfriendwidget->addWidget(pcb);
        mbuttongroup->addButton(pcb);
    }
    m_PSA->setWidget(m_pFriendW);
}

void sharefile::cancleselect()
{
    QList<QAbstractButton*>  cblist = mbuttongroup->buttons();
    for(int i = 0; i < cblist.size(); ++i){
        if(cblist[i]->isChecked()){
            cblist[i]->setChecked(false);
        }
    }
}

void sharefile::selectall()
{
    QList<QAbstractButton*>  cblist = mbuttongroup->buttons();
    for(int i = 0; i < cblist.size(); ++i){
        if(!cblist[i]->isChecked()){
            cblist[i]->setChecked(true);
        }
    }
}

void sharefile::choseok()
{
    QString name = Tcpclient::getinstance().getlogname();
    QString curpath = Tcpclient::getinstance().getcurpath();
    QString sharefilename = openwidget::getinstance().getbook()->getsharename();
    QString Path = curpath + "/" + sharefilename;
    QList<QAbstractButton*>  cblist = mbuttongroup->buttons();
    int num = 0;
    for(int i = 0; i < cblist.size(); ++i){
        if(cblist[i]->isChecked()){
            num++;
        }
    }
    pdu *Pdu = mkpud(32*num+Path.size()+1);
    Pdu->uiMsgType = msg_sharefile_request;
    sprintf(Pdu->caData,"%s %d",name.toStdString().c_str(),num);
    for(int i = 0,j = 0; i < cblist.size(); ++i){
        if(cblist[i]->isChecked()){
            memcpy((char*)(Pdu->caMsg)+j*32,cblist[i]->text().toStdString().c_str(),cblist[i]->text().size());
            j++;
        }
    }
    memcpy((char*)(Pdu->caMsg)+num*32,Path.toStdString().c_str(),Path.size());
    Tcpclient::getinstance().gettcpsocket().write((char*)Pdu,Pdu->uiPDUlen);
    free(Pdu);
    hide();
    Pdu = NULL;
}

void sharefile::chosecancle()
{
    hide();
}
