#include "book.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QMessageBox>

book::book(QWidget *parent)
    : QWidget{parent}
{
    m_booklist = new QListWidget;
    m_return = new QPushButton("返回");
    m_createdir = new QPushButton("创建文件夹");
    m_deldir = new QPushButton("删除文件夹");
    m_renamedir = new QPushButton("重命名");
    m_flushdir = new QPushButton("刷新文件夹");

    QVBoxLayout *pdir = new QVBoxLayout;
    pdir->addWidget(m_return);
    pdir->addWidget(m_createdir);
    pdir->addWidget(m_deldir);
    pdir->addWidget(m_renamedir);
    pdir->addWidget(m_flushdir);

    m_uploadfile = new QPushButton("上传文件");
    m_delfile = new QPushButton("删除文件");
    m_downloadfile = new QPushButton("下载文件");
    m_sharefile = new QPushButton("分享文件");
    QVBoxLayout *pfile = new QVBoxLayout;
    pfile->addWidget(m_uploadfile);
    pfile->addWidget(m_delfile);
    pfile->addWidget(m_downloadfile);
    pfile->addWidget(m_sharefile);

    QHBoxLayout *pmain = new QHBoxLayout;
    pmain->addWidget(m_booklist);
    pmain->addLayout(pdir);
    pmain->addLayout(pfile);

    setLayout(pmain);

    connect(m_createdir,SIGNAL(clicked(bool)),this,SLOT(creatdir()));
    connect(m_flushdir,SIGNAL(clicked(bool)),this,SLOT(flushfile()));
}

void book::handflushfile(pdu *Pdu)  //处理请求
{
    if(Pdu == NULL){
        return;
    }
    m_booklist->clear();
    fileinfo *tmpfileinfo = NULL;
    int len = Pdu->uiMsgLen/sizeof(fileinfo);
    for(int i = 0; i < len; ++i){
        tmpfileinfo = (fileinfo*)(Pdu->caMsg)+i;
        QListWidgetItem *tmp = new QListWidgetItem;
        if(0 == tmpfileinfo->filetype){
            tmp->setIcon(QIcon(QPixmap(":/icon/dir.png")));
        }else if(1 == tmpfileinfo->filetype){
            tmp->setIcon(QIcon(QPixmap(":/icon/reg.png")));
        }
        tmp->setText(tmpfileinfo->filename);
        m_booklist->addItem(tmp);
    }
}

void book::creatdir()
{
    QString newdir = QInputDialog::getText(this,"新建文件夹","新文件夹名");
    if(!newdir.isEmpty()){
        if(newdir.size() > 32){
            QMessageBox::warning(this,"新建文件夹","文件夹名字过长!");
        }else{
            QString logname = Tcpclient::getinstance().getlogname();
            QString curpath = Tcpclient::getinstance().getcurpath();
            pdu *Pdu = mkpud(newdir.size()+1);
            Pdu->uiMsgType = msg_creat_dir_request;
            strncpy(Pdu->caData,logname.toStdString().c_str(),logname.size());
            strncpy(Pdu->caData+32,newdir.toStdString().c_str(),newdir.size());
            memcpy(Pdu->caMsg,curpath.toStdString().c_str(),curpath.size());
            Tcpclient::getinstance().gettcpsocket().write((char*)Pdu,Pdu->uiPDUlen);
            free(Pdu);
            Pdu = NULL;
        }


    }else{
        QMessageBox::warning(this,"新建文件夹","文件夹名字不能为空！");
    }


}

void book::flushfile()  //发送请求
{
    QString curpath = Tcpclient::getinstance().getcurpath();
    pdu *Pdu = mkpud(curpath.size()+1);
    Pdu->uiMsgType = msg_flushfile_request;
    strncpy((char*)Pdu->caMsg,curpath.toStdString().c_str(),curpath.size());
    Tcpclient::getinstance().gettcpsocket().write((char*)Pdu,Pdu->uiPDUlen);
    free(Pdu);
    Pdu = NULL;
}
