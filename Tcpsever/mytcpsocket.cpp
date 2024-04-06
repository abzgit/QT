#include "mytcpsocket.h"
#include<QDebug>
#include "opdb.h"
#include <QMessageBox>
#include "mytcpserver.h"
#include <QDir>
#include <QFileInfoList>

MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}
{
    connect(this,SIGNAL(readyRead()),this,SLOT(recvmsg()));
    connect(this,SIGNAL(disconnected()),this,SLOT(clientoffline()));
    m_upload = false;
    mtimer = new QTimer;
    connect(mtimer,SIGNAL(timeout()),this,SLOT(sendfile()));
}

QString MyTcpSocket::getname()
{
    return m_name;
}

void MyTcpSocket::handregist(pdu *Pdu)
{
    char name[32] = {'\0'};
    char pwd[32] = {'\0'};
    strncpy(name,Pdu->caData,32);
    strncpy(pwd,Pdu->caData+32,32);
    bool ret = opDB::getinstance().handregist(name,pwd);
    pdu *respon= mkpud(0);
    respon->uiMsgType = msg_type_regist_respond;
    if(ret){
        strcpy(respon->caData,REGIST_OK);
        QDir dir;
    }else{
        strcpy(respon->caData,REGIST_FAILED);
    }
    write((char*)respon,respon->uiPDUlen);
    free(respon);
    respon = NULL;
}

void MyTcpSocket::handlogin(pdu *Pdu)
{
    char name[32] = {'\0'};
    char pwd[32] = {'\0'};
    strncpy(name,Pdu->caData,32);
    strncpy(pwd,Pdu->caData+32,32);
    bool ret = opDB::getinstance().handlogin(name,pwd);
    pdu *respon= mkpud(0);
    respon->uiMsgType = msg_type_login_respond;
    if(ret){
        strcpy(respon->caData,LOGIN_OK);
        m_name = name;
        QDir dir;
        dir.mkdir(QString("./%1").arg(name));
    }else{
        strcpy(respon->caData,LOGIN_FAILED);
    }
    write((char*)respon,respon->uiPDUlen);
    free(respon);
    respon = NULL;
}

void MyTcpSocket::handoline()
{
    QStringList ret = opDB::getinstance().handonline();
    uint len = ret.size()*32;
    pdu *Pdu1 = mkpud(len);
    Pdu1->uiMsgType = msg_type_online_respond;
    for(int i = 0; i < ret.size(); ++i){
        memcpy((char*)(Pdu1->caMsg)+i*32,ret[i].toStdString().c_str(),ret[i].size());
    }
    write((char*)Pdu1,Pdu1->uiPDUlen);
    free(Pdu1);
    Pdu1 = NULL;
}

void MyTcpSocket::handsearch(pdu *Pdu)
{
    int ret = opDB::getinstance().handsearch(Pdu->caData);
    pdu *respon = mkpud(0);
    respon->uiMsgType = msg_type_search_respond;
    if(ret == -1){
        strcpy(respon->caData,SEARCH_USER_EMPTY);
    }else if(ret == 1){
        strcpy(respon->caData,SEARCH_USER_OK);
    }else{
        strcpy(respon->caData,SEARCH_USER_OFFLINE);
    }
    write((char*)respon,respon->uiPDUlen);
    free(respon);
    respon = NULL;
}

void MyTcpSocket::handaddfriend(pdu *Pdu)
{
    char pname[32] = {'\0'};
    char mname[32] = {'\0'};
    strncpy(pname,Pdu->caData,32);
    strncpy(mname,Pdu->caData+32,32);
    int ret = opDB::getinstance().handadd(pname,mname);
    pdu *respon = mkpud(0);
    respon->uiMsgType = msg_type_addfri_respond;
    if(ret == -1){
        strcpy(respon->caData,UNKNOW_ERROR);
    }else if(0 == ret){
        strcpy(respon->caData,EXEITED_FRIEND);
    }else if(1 == ret){
        mytcpserver::getinstace().transend(pname,Pdu);
    }else if(2 == ret){
        strcpy(respon->caData,ADD_FRIEND_OFFLINE);
    }else if(3 == ret){
        strcpy(respon->caData,ADD_FRIEND_NO_EXIT);
    }
    write((char*)respon,respon->uiPDUlen);
    free(respon);
    respon = NULL;
}

void MyTcpSocket::handagreeadd(pdu *Pdu)
{
    char addedName[32] = {'\0'};
    char sourceName[32] = {'\0'};
    // 拷贝读取的信息
    strncpy(addedName, Pdu -> caData, 32);
    strncpy(sourceName, Pdu -> caData + 32, 32);
    opDB::getinstance().handagreeadd(addedName,sourceName);
    mytcpserver::getinstace().transend(sourceName,Pdu);
}

void MyTcpSocket::handflush(pdu *Pdu)
{
    char cname[32] = {'\0'};
    strncpy(cname,Pdu->caData,32);
    QStringList ans = opDB::getinstance().handflush(cname);
    uint uimsglen = ans.size()*32;
    pdu *respon = mkpud(uimsglen);
    for(int i = 0; i < ans.size(); ++i){
        memcpy((char *)(respon->caMsg)+i*32,ans.at(i).toStdString().c_str(),ans.at(i).size());
    }
    respon->uiMsgType = msg_flush_respon;
    write((char*)respon,respon->uiPDUlen);
    free(respon);
    respon = NULL;
}

void MyTcpSocket::handdelfriend(pdu *Pdu)
{
    char mName[32] = {'\0'};
    char fName[32] = {'\0'};
    // 拷贝读取的信息
    strncpy(mName, Pdu -> caData, 32);
    strncpy(fName, Pdu -> caData + 32, 32);
    opDB::getinstance().handdelfriend(mName,fName);
    pdu *respon = mkpud(0);
    respon->uiMsgType = msg_delfriend_respon;
    strcpy(respon->caData,DEL_FRIEND_OK);
    write((char*)respon,respon->uiPDUlen);
    mytcpserver::getinstace().transend(fName,Pdu);
    free(respon);
    respon = NULL;
}

void MyTcpSocket::handprivatechat(pdu *Pdu)
{
    char fname[32] = {'\0'};
    memcpy(fname,Pdu->caData+32,32);
    QString str = QString::fromLocal8Bit(fname);
    QStringList list = str.split(" ");
    QString newfname = list[0];
    strcpy(fname,newfname.toStdString().c_str());
    mytcpserver::getinstace().transend(fname,Pdu);
    qDebug()<<fname;
}

void MyTcpSocket::handgroupchat(pdu *Pdu)
{
    char caname[32] = {'\0'};
    strncpy(caname,Pdu->caData,32);
    QStringList online = opDB::getinstance().handflush(caname);
    QString tmp;
    for(int i = 0; i < online.size(); ++i){
        tmp = online[i].split(" ").at(0);
        mytcpserver::getinstace().transend(tmp.toStdString().c_str(),Pdu);
    }
}

void MyTcpSocket::handcreatdir(pdu *Pdu)
{
    QDir dir;
    QString curpath = QString("%1").arg((char*)Pdu->caMsg);
    pdu *respon = mkpud(0);
    respon->uiMsgType = msg_creat_dir_respon;
    bool ret = dir.exists(curpath);     //文件夹是否存在
    if(ret){
        char creatpath[32];
        memcpy(creatpath,Pdu->caData+32,32);
        QString newpath = curpath + "/" + creatpath;
        ret = dir.exists(newpath);
        if(ret){    //新路径是否存在
            strcpy(respon->caData,FILE_NAME_EXITED);
        }else{
            dir.mkdir(newpath);
            strcpy(respon->caData,CREATE_DIR_OK);
        }
    }else{
        strcpy(respon->caData,DIR_NO_EXIT);
    }
    write((char*)respon,respon->uiPDUlen);
    free(respon);
    respon = NULL;
}

void MyTcpSocket::handflushfile(pdu *Pdu)
{
    char *curpath = new char[Pdu->uiMsgLen];
    memcpy(curpath,Pdu->caMsg,Pdu->uiMsgLen);
    QDir dir(curpath);
    QFileInfoList filelist = dir.entryInfoList();
    int filecount = filelist.size();
    pdu *respon = mkpud(sizeof(fileinfo)*(filecount-2));
    respon->uiMsgType = msg_flushfile_respon;
    fileinfo *tmpinfo = NULL;
    QString tmpfilename;
    for(int i = 2; i < filecount; ++i){
        tmpinfo = (fileinfo*)(respon->caMsg)+i-2;
        tmpfilename = filelist[i].fileName();
        memcpy(tmpinfo->filename,tmpfilename.toStdString().c_str(),tmpfilename.size());
        if(filelist[i].isDir()){
            tmpinfo->filetype = 0;
        }else if(filelist[i].isFile()){
            tmpinfo->filetype = 1;
        }
    }
    write((char*)respon,respon->uiPDUlen);
    free(respon);
    respon = NULL;
}

void MyTcpSocket::handdeldir(pdu *Pdu)
{
    char dirname[32] = {'\0'};
    strcpy(dirname,Pdu->caData);
    char *path = new char[Pdu->uiMsgLen];
    memcpy(path,Pdu->caMsg,Pdu->uiMsgLen);
    QString nowpath = QString("%1/%2").arg(path).arg(dirname);
    QFileInfo fileinfo(nowpath);
    bool ret = false;
    if(fileinfo.isDir()){
        QDir dir;
        dir.setPath(nowpath);
        ret = dir.removeRecursively();
    }else if(fileinfo.isFile()){
        ret = false;
    }
    pdu *respon = mkpud(0);
    respon->uiMsgType = msg_deldir_respon;
    if(ret){
        memcpy(respon->caData,DEL_DIR_OK,strlen(DEL_DIR_OK));
    }else{
        memcpy(respon->caData,DEL_DIR_FAILURED,strlen(DEL_DIR_FAILURED));
    }
    write((char*)respon,respon->uiPDUlen);
    free(respon);
    respon = NULL;
}

void MyTcpSocket::handrename(pdu *Pdu)
{
    char oldname[32] = {'\0'};
    char newname[32] = {'\0'};
    strncpy(oldname,Pdu->caData,32);
    strncpy(newname,Pdu->caData+32,32);
    char *curpath = new char[Pdu->uiMsgLen];
    memcpy(curpath,Pdu->caMsg,Pdu->uiMsgLen);
    QString oldpath = QString("%1/%2").arg(curpath).arg(oldname);
    QString newpath = QString("%1/%2").arg(curpath).arg(newname);
    QDir dir;
    bool ret = dir.rename(oldpath,newpath);
    pdu *respon = mkpud(0);
    respon->uiMsgType = msg_rename_dir_respon;
    if(ret){
        strcpy(respon->caData,RENAME_FILE_OK);
    }else{
        strcpy(respon->caData,RENAME_FILE_FAILURED);
    }
    write((char*)respon,respon->uiPDUlen);
    free(respon);
    respon = NULL;
}

void MyTcpSocket::handenterdir(pdu *Pdu)
{
    char enterdir[32] = {'\0'};
    strncpy(enterdir,Pdu->caData,32);
    char *curpath = new char[Pdu->uiMsgLen];
    memcpy(curpath,Pdu->caMsg,Pdu->uiMsgLen);
    QString newpath = QString("%1/%2").arg(curpath).arg(enterdir);
    QFileInfo Fileinfo(newpath);
    pdu *respon = NULL;
    if(Fileinfo.isDir()){
        QDir dir(newpath);
        QFileInfoList filelist = dir.entryInfoList();
        int filecount = filelist.size();
        respon = mkpud(sizeof(fileinfo)*(filecount-2));
        respon->uiMsgType = msg_flushfile_respon;
        fileinfo *tmpinfo = NULL;
        QString tmpfilename;
        for(int i = 2; i < filecount; ++i){
            tmpinfo = (fileinfo*)(respon->caMsg)+i-2;
            tmpfilename = filelist[i].fileName();
            memcpy(tmpinfo->filename,tmpfilename.toStdString().c_str(),tmpfilename.size());
            qDebug()<<tmpfilename;
            if(filelist[i].isDir()){
                tmpinfo->filetype = 0;
            }else if(filelist[i].isFile()){
                tmpinfo->filetype = 1;
            }
        }
        write((char*)respon,respon->uiPDUlen);
        free(respon);
        respon = NULL;
    }else{
        respon = mkpud(0);
        respon->uiMsgType = msg_enter_dir_respon;
        strcpy(respon->caData,ENTER_DIR_FAILURED);
        write((char*)respon,respon->uiPDUlen);
        free(respon);
        respon = NULL;
    }
}

void MyTcpSocket::handupload(pdu *Pdu)
{
    char dirname[32] = {'\0'};
    qint64 filesize = 0;
    sscanf(Pdu->caData,"%s %lld",dirname,&filesize);
    char *path = new char[Pdu->uiMsgLen];
    memcpy(path,Pdu->caMsg,Pdu->uiMsgLen);
    QString nowpath = QString("%1/%2").arg(path).arg(dirname);
    delete[] path;
    path = NULL;
    m_file.setFileName(nowpath);
    //以只写的方式打开文件
    if(m_file.open(QIODevice::WriteOnly)){
        m_upload = true;
        m_total = filesize;
        m_recived = 0;

    }
}

void MyTcpSocket::handnotfile()
{
    uint uiPduLen = 0;
    this->read((char*)&uiPduLen,sizeof(uint));
    uint uiMsgLen = uiPduLen - sizeof(pdu);
    pdu *Pdu = mkpud(uiMsgLen);
    this->read((char*)Pdu + sizeof(uint),uiPduLen - sizeof(uint));
    switch (Pdu->uiMsgType) {
    case msg_type_regist_request:
    {
        handregist(Pdu);
        break;
    }
    case msg_type_login_request:
    {
        handlogin(Pdu);
        break;
    }
    case msg_type_online_request:{
        handoline();
        break;
    }
    case msg_type_search_request:{
        handsearch(Pdu);
        break;
    }
    case msg_type_addfri_request:{
        handaddfriend(Pdu);
        break;
    }
    case msg_type_agree_add_friend:{
        handagreeadd(Pdu);
        break;
    }
    case msg_typr_disagree_add:{
        char sourceName[32] = {'\0'};
        // 拷贝读取的信息
        strncpy(sourceName, Pdu -> caData + 32, 32);
        // 服务器需要转发给发送好友请求方其被拒绝的消息
        mytcpserver::getinstace().transend(sourceName,Pdu);
        break;
    }
    case msg_flush_request:{
        handflush(Pdu);
        break;
    }
    case msg_delfriend_request:{
        handdelfriend(Pdu);
        break;
    }
    case msg_privatechat_request:{
        handprivatechat(Pdu);
        break;
    }
    case msg_group_chat_request:{
        handgroupchat(Pdu);
        break;
    }
    case msg_creat_dir_request:{
        handcreatdir(Pdu);
        break;
    }
    case msg_flushfile_request:{
        handflushfile(Pdu);
        break;
    }
    case msg_deldir_request:{
        handdeldir(Pdu);
        break;
    }
    case msg_rename_dir_request:{
        handrename(Pdu);
        break;
    }
    case msg_enter_dir_request:{
        handenterdir(Pdu);
        break;
    }
    case msg_upload_request:{
        handupload(Pdu);
        break;
    }
    case msg_delfile_request:{
        handdelfile(Pdu);
        break;
    }
    case msg_download_request:{
        handdownload(Pdu);
        break;
    }
    case msg_sharefile_request:{
        handsharefile(Pdu);
        break;
    }
    case msg_sharefile_note_respon:{
        handshareyes(Pdu);
        break;
    }
    case msg_movefile_request:{
        handmove(Pdu);
        break;
    }
    default:
        break;
    }
    free(Pdu);
    Pdu = NULL;
}

void MyTcpSocket::handfile()
{
    pdu *respon = NULL;
    respon = mkpud(0);
    respon->uiMsgType = msg_uploadr_respon;
    QByteArray buff = readAll();
    m_file.write(buff);
    m_recived += buff.size();
    if(m_recived == m_total){
        m_file.close();
        m_upload = false;
        strcpy(respon->caData,UPLOAD_FILE_OK);
        write((char*)respon,respon->uiPDUlen);
        free(respon);
        respon = NULL;
    }else if(m_recived > m_total){
        strcpy(respon->caData,UPLOAD_FILE_FAILURED);
        m_file.close();
        m_upload = false;
        write((char*)respon,respon->uiPDUlen);
        free(respon);
        respon = NULL;
    }
}

void MyTcpSocket::handdelfile(pdu *Pdu)
{
    char dirname[32] = {'\0'};
    strcpy(dirname,Pdu->caData);
    char *path = new char[Pdu->uiMsgLen];
    memcpy(path,Pdu->caMsg,Pdu->uiMsgLen);
    QString nowpath = QString("%1/%2").arg(path).arg(dirname);
    QFileInfo fileinfo(nowpath);
    bool ret = false;
    if(fileinfo.isDir()){
        ret = false;
    }else if(fileinfo.isFile()){
        QDir dir;
        ret = dir.remove(nowpath);
    }
    pdu *respon = mkpud(0);
    respon->uiMsgType = msg_delfile_respon;
    if(ret){
        memcpy(respon->caData,DEL_FILE_OK,strlen(DEL_FILE_OK));
    }else{
        memcpy(respon->caData,DEL_FILE_FAILURED,strlen(DEL_FILE_FAILURED));
    }
    write((char*)respon,respon->uiPDUlen);
    free(respon);
    respon = NULL;
}

void MyTcpSocket::handdownload(pdu *Pdu)
{
    char dirname[32] = {'\0'};
    strcpy(dirname,Pdu->caData);
    char *path = new char[Pdu->uiMsgLen];
    memcpy(path,Pdu->caMsg,Pdu->uiMsgLen);
    QString nowpath = QString("%1/%2").arg(path).arg(dirname);
    QFileInfo Fileinfo(nowpath);
    qint64 filesize = Fileinfo.size();
    pdu *respon = mkpud(0);
    respon->uiMsgType = msg_download_respon;
    m_file.setFileName(nowpath);
    sprintf(respon->caData,"%s %lld",dirname,filesize);
    write((char*)respon,respon->uiPDUlen);
    free(respon);
    respon = NULL;
    mtimer->start(1000);
    delete[] path;
    path = NULL;
}

void MyTcpSocket::handsharefile(pdu *Pdu)
{
    char sendname[32] = {'\0'};
    int num = 0;
    sscanf(Pdu->caData,"%s %d",sendname,&num);
    int size = num*32;
    pdu *respon = mkpud(Pdu->uiMsgLen-size);
    respon->uiMsgType = msg_sharefile_note;
    strcpy(respon->caData,sendname);
    memcpy(respon->caMsg,(char*)(Pdu->caMsg)+size,Pdu->uiMsgLen-size);
    char recvname[32] = {'\0'};
    for(int i = 0; i < num; ++i){
        memcpy(recvname,(char*)(Pdu->caMsg)+i*32,32);
        mytcpserver::getinstace().transend(recvname,respon);
    }
    free(respon);
    respon = NULL;
    respon = mkpud(0);
    respon->uiMsgType = msg_sharefile_respon;
    strcpy(Pdu->caData,"share file ok!");
    write((char*)respon,respon->uiPDUlen);
    free(respon);
    respon = NULL;
}

void MyTcpSocket::handshareyes(pdu *Pdu)
{
    QString recvpath = QString("./%1").arg(Pdu->caData);
    QString sharepath = QString("%1").arg((char*)(Pdu->caMsg));
    int index = sharepath.lastIndexOf('/');
    QString filename = sharepath.right(sharepath.size()-index-1);
    recvpath = recvpath + "/" + filename;
    QFileInfo file(sharepath);
    if(file.isFile()){
        QFile::copy(sharepath,recvpath);
    }else if(file.isDir()){
        handshardir(sharepath,recvpath);
    }
}

void MyTcpSocket::handshardir(QString srcdir, QString desdir)
{
    QDir dir;
    dir.mkdir(desdir);
    dir.setPath(srcdir);
    QFileInfoList fileinfolist = dir.entryInfoList();
    QString srctmp,destmp;
    for(int i = 0; i < fileinfolist.size(); ++i){
        if(fileinfolist[i].isFile()){
            srctmp = srcdir + '/' + fileinfolist[i].fileName();
            destmp = desdir + '/' + fileinfolist[i].fileName();
            QFile::copy(srctmp,destmp);
        }else if(fileinfolist[i].isDir()){
            srctmp = srcdir + '/' + fileinfolist[i].fileName();
            destmp = desdir + '/' + fileinfolist[i].fileName();
            handshardir(srctmp,destmp);
        }
    }
}

void MyTcpSocket::handmove(pdu *Pdu)
{
    char filename[32] = {'\0'};
    int srclen = 0;
    int deslen = 0;
    sscanf(Pdu->caData,"%d %d %s",&srclen,&deslen,filename);
    char *srcpath = new char[srclen+1];
    char *despath = new char[deslen+1+32];
    memset(srcpath,'\0',srclen+1);
    memset(despath,'\0',deslen+1+32);
    memcpy(srcpath,Pdu->caMsg,srclen);
    memcpy(despath,(char*)(Pdu->caMsg) + (srclen + 1),deslen);
    pdu *respon = mkpud(0);
    respon->uiMsgType = msg_movefile_respon;
    QFileInfo file(despath);
    if(file.isDir()){
        strcat(despath,"/");
        strcat(despath,filename);
        bool ret = QFile::rename(srcpath,despath);
        if(ret){
            strcpy(respon->caData,MOVE_FILE_OK);
        }else{
            strcpy(respon->caData,COMMON_ERROR);
        }
    }else if(file.isFile()){
        strcpy(respon->caData,MOVE_FILE_FAILURED);
    }
    write((char*)respon,respon->uiPDUlen);
    free(respon);
    respon = NULL;
}

void MyTcpSocket::recvmsg()
{
    if(!m_upload){
        handnotfile();
    }else{
        handfile();
    }
}

void MyTcpSocket::clientoffline()
{
    opDB::getinstance().handoffline(m_name.toStdString().c_str());
    emit offline(this);

}

void MyTcpSocket::sendfile()
{
    mtimer->stop();
    m_file.open(QIODevice::ReadOnly);
    char *pdata = new char[4096];
    qint64 ret = 0;
    while(true){
        ret = m_file.read(pdata,4096);
        if(ret > 0 && ret <= 4096){
            write(pdata,ret);
        }else if(0 == ret){
            m_file.close();
            break;
        }else if(ret < 0){
            m_file.close();
            break;
        }
    }
    delete[] pdata;
    pdata = NULL;
}
