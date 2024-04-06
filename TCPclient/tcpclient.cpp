#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QFile>
#include <QMessageBox>
#include "openwidget.h"
#include <QDebug>

Tcpclient::Tcpclient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Tcpclient)
{
    ui->setupUi(this);
    loadconfig();
    connect(&m_sock,SIGNAL(connected()),this,SLOT(showconnect()));
    connect(&m_sock,SIGNAL(readyRead()),this,SLOT(recvmag()));
    m_sock.connectToHost(QHostAddress(m_ip),m_port);
}

Tcpclient::~Tcpclient()
{
    delete ui;
}

void Tcpclient::loadconfig()
{
    QFile file(":/clientconfig.txt");
    if(file.open(QIODevice::ReadOnly)){
        QByteArray badata = file.readAll();
        QString strdata = badata.toStdString().c_str();
        strdata.replace("\r\n"," ");
        QStringList datas = strdata.split(" ");
        m_ip = datas.at(0);
        m_port = datas.at(1).toUShort();
        file.close();
    }else{
        qDebug()<<"error";
    }
}

Tcpclient &Tcpclient::getinstance()
{
    static Tcpclient instance;
    return instance;
}

QTcpSocket &Tcpclient::gettcpsocket()
{
    return m_sock;
}

QString Tcpclient::getlogname()
{
    return logname;
}

QString Tcpclient::getcurpath()
{
    return m_curpath;
}

void Tcpclient::handregist(pdu *Pdu)
{
    if(0 == strcmp(Pdu->caData,REGIST_OK)){
        QMessageBox::information(this,"注册","注册成功!");
    }else if(0 == strcmp(Pdu->caData,REGIST_FAILED)){
        QMessageBox::warning(this,"注册","regist failed!name exited!");
    }
}

void Tcpclient::handlogin(pdu *Pdu)
{
    if(0 == strcmp(Pdu->caData,LOGIN_OK)){
        QMessageBox::information(this,"登录","登录成功");
        m_curpath = QString("./%1").arg(logname);
        openwidget::getinstance().show();
        this->hide();
    }else if(0 == strcmp(Pdu->caData,LOGIN_FAILED)){
        QMessageBox::warning(this,"登录","用户名或密码错误!或已经登录");
    }
}

void Tcpclient::handsearch(pdu *Pdu)
{
    if(0 == strcmp(Pdu->caData,SEARCH_USER_EMPTY)){
        QMessageBox::information(this,"搜索",QString("%1不存在！").arg(openwidget::getinstance().getfriend()->m_searchname));
    }else if(0 == strcmp(Pdu->caData,SEARCH_USER_OK)){
        QMessageBox::information(this,"搜索",QString("%1在线！").arg(openwidget::getinstance().getfriend()->m_searchname));
    }else if(0 == strcmp(Pdu->caData,SEARCH_USER_OFFLINE)){
        QMessageBox::information(this,"搜索",QString("%1不在线！").arg(openwidget::getinstance().getfriend()->m_searchname));
    }
}

void Tcpclient::handaddrequest(pdu *Pdu)
{
    char caname[32] = {'\0'};   //发送方名字
    strncpy(caname,Pdu->caData + 32,32);
    int ret = QMessageBox::information(this,"添加好友",
                                       QString("%1 want to add you as friend?").arg(caname),QMessageBox::Yes,QMessageBox::No);
    pdu *respon = mkpud(0);
    memcpy(respon->caData,Pdu->caData,32);
    memcpy(respon->caData+32,Pdu->caData+32,32);
    if(ret == QMessageBox::Yes){
        respon->uiMsgType = msg_type_agree_add_friend;
    }else{
        respon->uiMsgType = msg_typr_disagree_add;
    }
    m_sock.write((char*)respon,respon->uiPDUlen);
    free(respon);
    respon = NULL;
}

void Tcpclient::setcurpath(QString curpath)
{
    m_curpath = curpath;
}

void Tcpclient::handdownload(pdu *Pdu)
{
    char filename[32] = {'\0'};
    sscanf(Pdu->caData,"%s %lld",filename,&(openwidget::getinstance().getbook()->total));
    if(strlen(filename) > 0 && openwidget::getinstance().getbook()->total > 0){
        openwidget::getinstance().getbook()->setdownload(true);
        m_file.setFileName(openwidget::getinstance().getbook()->getsavepath());
        if(!m_file.open(QIODevice::WriteOnly)){
            QMessageBox::warning(this,"下载文件","获得保存文件路径失败");
        }
    }
}

void Tcpclient::handsharefile(pdu *Pdu)
{
    char *path = new char[Pdu->uiMsgLen];
    memcpy(path,Pdu->caMsg,Pdu->uiMsgLen);
    char * index = strrchr(path,'/');
    if(NULL != index){
        index++;
        QString strnode = QString("%1 share file ->%2\n Do you accept?").arg(Pdu->caData).arg(index);
        int ret = QMessageBox::question(this,"共享文件",strnode);
        if(QMessageBox::Yes == ret){
            pdu *respon =mkpud(Pdu->uiMsgLen);
            respon->uiMsgType = msg_sharefile_note_respon;
            memcpy(respon->caMsg,Pdu->caMsg,Pdu->uiMsgLen);
            QString name = Tcpclient::getinstance().getlogname();
            strcpy(respon->caData,name.toStdString().c_str());
            m_sock.write((char*)respon,respon->uiPDUlen);
            free(respon);
            respon = NULL;
        }
    }
}


void Tcpclient::showconnect()
{
    QMessageBox::information(this,"连接服务器","连接成功");
}

void Tcpclient::recvmag()
{
    if(!openwidget::getinstance().getbook()->getstatu()){
    uint uiPduLen = 0;
    m_sock.read((char*)&uiPduLen,sizeof(uint));
    uint uiMsgLen = uiPduLen - sizeof(pdu);
    pdu *Pdu = mkpud(uiMsgLen);
    m_sock.read((char*)Pdu + sizeof(uint),uiPduLen - sizeof(uint));
    switch (Pdu->uiMsgType) {
    case msg_type_regist_respond:
    {
        handregist(Pdu);
        break;
    }
    case msg_type_login_respond:
    {
        handlogin(Pdu);
        break;
    }
    case msg_type_online_respond:{
        openwidget::getinstance().getfriend()->showonlineuse(Pdu);
        break;
    }
    case msg_type_search_respond:{
        handsearch(Pdu);
        break;
    }
    case msg_type_addfri_request:{
        handaddrequest(Pdu);
        break;
    }
    case msg_type_addfri_respond:{
        QMessageBox::information(this,"添加好友",
                  "已发送好友请求!");
        break;
    }
    case msg_type_agree_add_friend:{
        QMessageBox::information(this, "添加好友", QString("%1 已同意您的好友申请！").arg(Pdu -> caData));
        break;
    }
    case msg_typr_disagree_add:{
        QMessageBox::information(this, "添加好友", QString("%1 已拒绝您的好友申请！").arg(Pdu -> caData));
        break;
    }
    case msg_flush_respon:{
        openwidget::getinstance().getfriend()->handflush(Pdu);
        break;
    }
    case msg_delfriend_request:{
        char fname[32] = {'\0'};
        memcpy(fname,Pdu->caData,32);
        QMessageBox::information(this,"删除好友",QString("%1 已删除删除与你的好友关系").arg(fname));
        break;
    }
    case msg_delfriend_respon:{
        QMessageBox::information(this,"删除好友","删除好友成功");
        break;
    }
    case msg_privatechat_request:{
        privatechat::getinsance().handprivatechat(Pdu);
        break;
    }
    case msg_group_chat_request:{
        openwidget::getinstance().getfriend()->handgroupchat(Pdu);
        break;
    }
    case msg_creat_dir_respon:{
        QMessageBox::information(this,"创建文件",(char*)(Pdu->caData));
        break;
    }
    case msg_flushfile_respon:{
        openwidget::getinstance().getbook()->handflushfile(Pdu);
        QString enter = openwidget::getinstance().getbook()->getenterdir();
        if(!enter.isEmpty()){
            m_curpath = m_curpath+"/"+enter;
        }
        break;
    }
    case msg_deldir_respon:{
        QMessageBox::information(this,"删除文件夹",(char*)(Pdu->caData));
        break;
    }
    case msg_rename_dir_respon:{
        QMessageBox::information(this,"重命名文件",(char*)(Pdu->caData));
        break;
    }
    case msg_enter_dir_respon:{
        openwidget::getinstance().getbook()->clearmenterdir();
        QMessageBox::information(this,"进入文件夹",(char*)(Pdu->caData));
        break;
    }
    case msg_uploadr_respon:{
        QMessageBox::information(this,"上传文件",(char*)(Pdu->caData));
        break;
    }
    case msg_delfile_respon:{
        QMessageBox::information(this,"删除文件",(char*)(Pdu->caData));
        break;
    }
    case msg_download_respon:{
        handdownload(Pdu);
        break;
    }
    case msg_sharefile_respon:{
        QMessageBox::information(this,"共享文件",(char*)(Pdu->caData));
        break;
    }
    case msg_sharefile_note:{
        handsharefile(Pdu);
        break;
    }
    case msg_movefile_respon:{
        QMessageBox::information(this,"移动文件",(char*)(Pdu->caData));
        break;
    }
    default:
        break;
    }
    free(Pdu);
    Pdu = NULL;
    }else{
        QByteArray buffer = m_sock.readAll();
        m_file.write(buffer);
        book *tmpbook = openwidget::getinstance().getbook();
        tmpbook->recived += buffer.size();
        if(tmpbook->recived == tmpbook->total){
            m_file.close();
            tmpbook->recived = 0;
            tmpbook->total = 0;
            tmpbook->setdownload(false);
        }else if(tmpbook->recived > tmpbook->total){
            m_file.close();
            tmpbook->recived = 0;
            tmpbook->total = 0;
            tmpbook->setdownload(false);
            QMessageBox::critical(this,"下载文件","下载文件失败");
        }
    }
}
#if 0
void Tcpclient::on_pushButton_clicked()
{
    QString msf = ui->lineEdit->text();
    if(!msf.isEmpty()){
        pdu *Pdu = mkpud(msf.size());
        memcpy(Pdu->caMsg,msf.toStdString().c_str(),msf.size());
        m_sock.write((char*)Pdu,Pdu->uiPDUlen);
        free(Pdu);
        Pdu = NULL;
    }else{
        QMessageBox::warning(this,"信息发送","信息不能为空");
    }
}
#endif

void Tcpclient::on_login_clicked()
{
    QString name = ui->line_le->text();
    QString pwd = ui->pwd_le->text();
    if(!name.isEmpty() && !pwd.isEmpty()){
        logname = name;
        pdu *Pdu = mkpud(0);
        Pdu->uiMsgType = msg_type_login_request;
        memcpy(Pdu->caData,name.toStdString().c_str(),32);
        memcpy(Pdu->caData+32,pwd.toStdString().c_str(),32);
        m_sock.write((char*)Pdu,Pdu->uiPDUlen);
        free(Pdu);
        Pdu = NULL;
    }else{
        QMessageBox::critical(this,"登录","用户名或密码不能为空！");
    }
}


void Tcpclient::on_regist_clicked()
{
    QString name = ui->line_le->text();
    QString pwd = ui->pwd_le->text();
    if(!name.isEmpty() && !pwd.isEmpty()){
        pdu *Pdu = mkpud(0);
        Pdu->uiMsgType = msg_type_regist_request;
        memcpy(Pdu->caData,name.toStdString().c_str(),32);
        memcpy(Pdu->caData+32,pwd.toStdString().c_str(),32);
        m_sock.write((char*)Pdu,Pdu->uiPDUlen);
        free(Pdu);
        Pdu = NULL;
    }else{
        QMessageBox::critical(this,"注册","注册失败，用户名或密码不能为空！");
    }
}


void Tcpclient::on_cancel_clicked()
{

}

