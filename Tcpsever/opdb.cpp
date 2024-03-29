#include "opdb.h"
#include <QMessageBox>
#include <QDebug>
#include <QResource>
#include <QSqlQuery>

opDB::opDB(QObject *parent)
    : QObject{parent}
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

opDB &opDB::getinstance()
{
    static opDB instance;
    return instance;
}

void opDB::initdb()
{
    m_db.setHostName("localhost");
    m_db.setDatabaseName("D:\\Qt\\Tcpsever\\cloud.db");
    if(m_db.open()){
        QSqlQuery que;
        que.exec("select * from useinfo");
        while(que.next()){
            QString str = QString("%1,%2,%3").arg(que.value(0).toString()).arg(que.value(1).toString()).arg(que.value(2).toString());
            qDebug()<<str;
        }
    }else{
        QMessageBox::critical(NULL,"打开数据库","打开数据库失败");
    }
}

opDB::~opDB()
{
    m_db.close();
}

bool opDB::handregist(const char *name, const char *pwd)
{
    if(name == NULL || pwd == NULL){
        return false;
    }
    QString sqlque = QString("insert into useinfo(name,pwd) values(\'%1\',\'%2\')").arg(name).arg(pwd);
    QSqlQuery query;
    return query.exec(sqlque);
}

bool opDB::handlogin(const char *name, const char *pwd)
{
    if(name == NULL || pwd == NULL){
        return false;
    }
    QString sqlque = QString("select * from useinfo where name = \'%1\' and pwd = \'%2\' and online = 0").arg(name).arg(pwd);
    QSqlQuery quey;
    quey.exec(sqlque);
    if(quey.next()){
        sqlque = QString("update useinfo set online = 1 where name = \'%1\' and pwd = \'%2\'").arg(name).arg(pwd);
        QSqlQuery quey;
        quey.exec(sqlque);
        return true;
    }else{
        return false;
    }
}

void opDB::handoffline(const char *name)
{
    if(name == NULL){
        qDebug()<<"name is NULL!";
        return;
    }
    QString sqlque = QString("update useinfo set online = 0 where name = \'%1\'").arg(name);
    QSqlQuery quey;
    quey.exec(sqlque);
}
