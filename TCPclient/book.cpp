#include "book.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "openwidget.h"
#include "sharefile.h"

book::book(QWidget *parent)
    : QWidget{parent}
{
    m_enterdir.clear();
    m_timer = new QTimer;
    isdownload = false;
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
    movefile = new QPushButton("移动文件");
    selectdesdir = new QPushButton("目标目录");
    selectdesdir->setEnabled(false);

    QVBoxLayout *pfile = new QVBoxLayout;
    pfile->addWidget(m_uploadfile);
    pfile->addWidget(m_delfile);
    pfile->addWidget(m_downloadfile);
    pfile->addWidget(m_sharefile);
    pfile->addWidget(movefile);
    pfile->addWidget(selectdesdir);

    QHBoxLayout *pmain = new QHBoxLayout;
    pmain->addWidget(m_booklist);
    pmain->addLayout(pdir);
    pmain->addLayout(pfile);

    setLayout(pmain);

    connect(m_createdir,SIGNAL(clicked(bool)),this,SLOT(creatdir()));
    connect(m_flushdir,SIGNAL(clicked(bool)),this,SLOT(flushfile()));
    connect(m_deldir,SIGNAL(clicked(bool)),this,SLOT(deldir()));
    connect(m_renamedir,SIGNAL(clicked(bool)),this,SLOT(renamedir()));
    connect(m_booklist,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(enterdir(QModelIndex)));
    connect(m_return,SIGNAL(clicked(bool)),this,SLOT(returnpredir()));
    connect(m_uploadfile,SIGNAL(clicked(bool)),this,SLOT(uploadfile()));
    connect(m_timer,SIGNAL(timeout()),this,SLOT(uploadfiledate()));
    connect(m_delfile,SIGNAL(clicked(bool)),this,SLOT(delfile()));
    connect(m_downloadfile,SIGNAL(clicked(bool)),this,SLOT(download()));
    connect(m_sharefile,SIGNAL(clicked(bool)),this,SLOT(sharefile()));
    connect(movefile,SIGNAL(clicked(bool)),this,SLOT(movefileordir()));
    connect(selectdesdir,SIGNAL(clicked(bool)),this,SLOT(selectdir()));
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

void book::setdownload(bool statu)
{
    isdownload = statu;
}

bool book::getstatu()
{
    return isdownload;
}

QString book::getsavepath()
{
    return m_savepath;
}

QString book::getsharename()
{
    return msharefilename;
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

void book::deldir()
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
    m_uploadfilepath = QFileDialog::getOpenFileName();
    if(!m_uploadfilepath.isEmpty()){
        int pos = m_uploadfilepath.lastIndexOf('/');
        QString filename = m_uploadfilepath.right(m_uploadfilepath.size()-pos-1);
        QFile file(m_uploadfilepath);
        qint64 filesize = file.size();
        pdu *Pdu = mkpud(curpath.size()+1);
        Pdu->uiMsgType = msg_upload_request;
        memcpy(Pdu->caMsg,curpath.toStdString().c_str(),curpath.size());
        sprintf(Pdu->caData,"%s %lld",filename.toStdString().c_str(),filesize);
        Tcpclient::getinstance().gettcpsocket().write((char*)Pdu,Pdu->uiPDUlen);
        free(Pdu);
        Pdu = NULL;
        m_timer->start(1000);
    }else{
        QMessageBox::warning(this,"上传文件","上传的文件不能为空！");
    }
}

void book::uploadfiledate()
{
    m_timer->stop();
    QFile file(m_uploadfilepath);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,"上传文件","打开文件失败");
        return;
    }
    char *buffer = new char[4096];
    qint64 ret = 0;
    while(true){
        ret = file.read(buffer,4096);
        if(ret > 0 && ret <= 4096){
            Tcpclient::getinstance().gettcpsocket().write(buffer,ret);
        }else if(ret == 0){
            break;
        }else{
            QMessageBox::warning(this,"上传文件","上传文件失败:读文件失败");
            break;
        }
    }
    file.close();
    delete[] buffer;
    buffer = NULL;
}

void book::delfile()
{
    QString curpath = Tcpclient::getinstance().getcurpath();
    QListWidgetItem *chose = m_booklist->currentItem();
    if(NULL == chose){
        QMessageBox::warning(this,"删除文件","请选择要删除的文件");
    }else{
        QString dirname = chose->text();
        pdu *Pdu = mkpud(curpath.size()+1);
        Pdu->uiMsgType = msg_delfile_request;
        strncpy(Pdu->caData,dirname.toStdString().c_str(),dirname.size());
        memcpy(Pdu->caMsg,curpath.toStdString().c_str(),curpath.size());
        Tcpclient::getinstance().gettcpsocket().write((char*)Pdu,Pdu->uiPDUlen);
        free(Pdu);
        Pdu = NULL;
    }
}

void book::download()
{
    QListWidgetItem *chose = m_booklist->currentItem();
    if(NULL == chose){
        QMessageBox::warning(this,"下载文件","请选择要下载的文件");
    }else{
        QString cursavapath = QFileDialog::getSaveFileName();
        if(cursavapath.isEmpty()){
            QMessageBox::warning(this,"下载文件","请指定保存文件的位置");
            m_savepath.clear();
        }else{
            m_savepath = cursavapath;
        }
        QString curpath = Tcpclient::getinstance().getcurpath();
        pdu *Pdu = mkpud(curpath.size()+1);
        Pdu->uiMsgType = msg_download_request;
        strcpy(Pdu->caData,chose->text().toStdString().c_str());
        memcpy(Pdu->caMsg,curpath.toStdString().c_str(),curpath.size());
        Tcpclient::getinstance().gettcpsocket().write((char*)Pdu,Pdu->uiPDUlen);
        free(Pdu);
        Pdu = NULL;
    }
}

void book::sharefile()
{
    QListWidgetItem *chose = m_booklist->currentItem();
    if(NULL == chose){
        QMessageBox::warning(this,"分享文件","请选择要分享的文件");
        return;
    }else{
        msharefilename = chose->text();
    }
    friends *pfriend = openwidget::getinstance().getfriend();
    QListWidget *friendlist = pfriend->getfriendlist();
    sharefile::getinstance().updatefriend(friendlist);
    if(sharefile::getinstance().isHidden()){
        sharefile::getinstance().show();
    }
}

void book::movefileordir()
{
    QListWidgetItem *item = m_booklist->currentItem();
    if(NULL != item){
        movefilename = item->text();
        QString curpath = Tcpclient::getinstance().getcurpath();
        movefilepath = curpath + '/' + movefilename;
        selectdesdir->setEnabled(true);


    }else{
        QMessageBox::warning(this,"移动文件","请选择要移动的文件");
    }
}

void book::selectdir()
{
    QListWidgetItem *item = m_booklist->currentItem();
    if(NULL != item){
        QString strdesdir  = item->text();
        QString curpath = Tcpclient::getinstance().getcurpath();
        desdir = curpath + '/' + strdesdir;
        int srclen = movefilepath.size();
        int deslen = desdir.size();
        pdu *Pdu = mkpud(srclen + deslen+2);
        Pdu->uiMsgType = msg_movefile_request;
        sprintf(Pdu->caData,"%d %d %s",srclen,deslen,movefilename.toStdString().c_str());
        memcpy(Pdu->caMsg,movefilepath.toStdString().c_str(),srclen);
        memcpy((char*)(Pdu->caMsg)+(srclen+1),desdir.toStdString().c_str(),deslen);
        Tcpclient::getinstance().gettcpsocket().write((char*)Pdu,Pdu->uiPDUlen);
        free(Pdu);
        Pdu = NULL;
    }else{
        QMessageBox::warning(this,"移动文件","请选择要移动的文件");
    }
    selectdesdir->setEnabled(false);
}
