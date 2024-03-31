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

void Tcpclient::showconnect()
{
    QMessageBox::information(this,"连接服务器","连接成功");
}

void Tcpclient::recvmag()
{
    uint uiPduLen = 0;
    m_sock.read((char*)&uiPduLen,sizeof(uint));
    uint uiMsgLen = uiPduLen - sizeof(pdu);
    pdu *Pdu = mkpud(uiMsgLen);
    m_sock.read((char*)Pdu + sizeof(uint),uiPduLen - sizeof(uint));
    qDebug()<<Pdu->uiMsgType;
    switch (Pdu->uiMsgType) {
    case msg_type_regist_respond:
    {
        qDebug()<<"inter";
        if(0 == strcmp(Pdu->caData,"regist ok!")){
            QMessageBox::information(this,"注册","注册成功!");
        }else if(0 == strcmp(Pdu->caData,"regist failed!name exited!")){
            QMessageBox::warning(this,"注册","regist failed!name exited!");
        }
        break;
    }
    case msg_type_login_respond:
    {
        if(0 == strcmp(Pdu->caData,"login ok!")){
            QMessageBox::information(this,"登录","登录成功");
            openwidget::getinstance().show();
            this->hide();
        }else if(0 == strcmp(Pdu->caData,"no account!")){
            QMessageBox::warning(this,"登录","用户名或密码错误!或已经登录");
        }
        break;
    }
    case msg_type_online_respond:{
        qDebug()<<msg_type_online_respond;
        openwidget::getinstance().getfriend()->showonlineuse(Pdu);
        qDebug()<<Pdu->uiMsgType;
        break;
    }
    default:
        break;
    }
    free(Pdu);
    Pdu = NULL;
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

