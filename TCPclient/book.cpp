#include "book.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>

book::book(QWidget *parent)
    : QWidget{parent}
{
    m_enterdir.clear();
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
    connect(m_deldir,SIGNAL(clicked(bool)),this,SLOT(delfit()));
    connect(m_renamedir,SIGNAL(clicked(bool)),this,SLOT(renamedir()));
    connect(m_booklist,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(enterdir(QModelIndex)));
    connect(m_return,SIGNAL(clicked(bool)),this,SLOT(returnpredir()));
    connect(m_uploadfile,SIGNAL(clicked(bool)),this,SLOT(uploadfile()));
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

void book::clearmenterdir()
{
    m_enterdir.clear();
}

QString book::getenterdir()
{
    return m_enterdir;
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

void book::delfit()
{
    QString curpath = Tcpclient::getinstance().getcurpath();
    QListWidgetItem *chose = m_booklist->currentItem();
    if(NULL == chose){
        QMessageBox::warning(this,"删除文件夹","请选择要删除的文件夹");
    }else{
        QString dirname = chose->text();
        pdu *Pdu = mkpud(curpath.size()+1);
        Pdu->uiMsgType = msg_deldir_request;
        strncpy(Pdu->caData,dirname.toStdString().c_str(),dirname.size());
        memcpy(Pdu->caMsg,curpath.toStdString().c_str(),curpath.size());
        Tcpclient::getinstance().gettcpsocket().write((char*)Pdu,Pdu->uiPDUlen);
        free(Pdu);
        Pdu = NULL;
    }
}

void book::renamedir()
{
    QString curpath = Tcpclient::getinstance().getcurpath();
    pdu *Pdu = NULL;
    QListWidgetItem *chose = m_booklist->currentItem();
    if(chose == NULL){
        QMessageBox::warning(this,"重命名文件","请选择要删除的文件");
    }else{
        QString oldname = chose->text();
        QString newname = QInputDialog::getText(this,"重命名文件","请输入新的文件名");
        if(!newname.isEmpty()){
            Pdu = mkpud(curpath.size()+1);
            Pdu->uiMsgType = msg_rename_dir_request;
            strncpy(Pdu->caData,oldname.toStdString().c_str(),oldname.size());
            strncpy(Pdu->caData+32,newname.toStdString().c_str(),newname.size());
            memcpy(Pdu->caMsg,curpath.toStdString().c_str(),curpath.size());
            Tcpclient::getinstance().gettcpsocket().write((char*)Pdu,Pdu->uiPDUlen);
            free(Pdu);
            Pdu = NULL;
        }else{
            QMessageBox::warning(this,"重命名文件","文件名不能为空");
        }
    }
}

void book::enterdir(const QModelIndex &index)
{
    QString dirname = index.data().toString();
    m_enterdir = dirname;
    QString curpath = Tcpclient::getinstance().getcurpath();
    pdu *Pdu = mkpud(curpath.size()+1);
    Pdu->uiMsgType = msg_enter_dir_request;
    strncpy(Pdu->caData,dirname.toStdString().c_str(),dirname.size());
    memcpy(Pdu->caMsg,curpath.toStdString().c_str(),curpath.size());
    Tcpclient::getinstance().gettcpsocket().write((char*)Pdu,Pdu->uiPDUlen);
    free(Pdu);
    Pdu = NULL;
}

void book::returnpredir()
{
    QString curpath = Tcpclient::getinstance().getcurpath();
    QString rootpath = "./"+Tcpclient::getinstance().getlogname();
    if(curpath == rootpath){
        QMessageBox::warning(this,"返回上层目录","根目录，不可返回！");
    }else{
        int pos = curpath.lastIndexOf('/');
        curpath.remove(pos,curpath.size()-pos);
        Tcpclient::getinstance().setcurpath(curpath);
        clearmenterdir();
        flushfile();
    }
}

void book::uploadfile()
{
    QString curpath = Tcpclient::getinstance().getcurpath();
    QString uploadfile = QFileDialog::getOpenFileName();
    if(!uploadfile.isEmpty()){
        int pos = uploadfile.lastIndexOf('/');
        QString filename = uploadfile.right(uploadfile.size()-pos-1);
        QFile file(uploadfile);
        qint64 filesize = file.size();
        pdu *Pdu = mkpud(curpath.size()+1);
        Pdu->uiMsgType = msg_upload_request;
        memcpy(Pdu->caMsg,curpath.toStdString().c_str(),curpath.size());
        sprintf(Pdu->caData,"%s %lld",filename.toStdString().c_str(),filesize);
        Tcpclient::getinstance().gettcpsocket().write((char*)Pdu,Pdu->uiPDUlen);
        free(Pdu);
        Pdu = NULL;
    }else{
        QMessageBox::warning(this,"上传文件","上传的文件不能为空！");
    }
}
