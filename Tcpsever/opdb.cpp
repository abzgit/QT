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

int opDB::handsearch(const char *name)
{
    if(name == NULL){
        return -1;
    }
    QString sqlsqe = QString("select online from useinfo where name = \'%1\'").arg(name);
    QSqlQuery que;
    que.exec(sqlsqe);
    if(que.next()){
        int ret = que.value(0).toInt();
        if(ret == 1){
            return 1;
        }else{
            return 0;
        }
    }else{
        return -1;
    }
}

int opDB::handadd(const char *fname, const char *mname)
{
    if(fname == NULL || mname == NULL){
        return -1;
    }
    QString sqlsqe = QString("select * from friendinfo where (id = (select id from useinfo where name = \'%1\') and friendid = (select id from useinfo where name = \'%2\')) "
                             "or (id = (select id from useinfo where name = \'%3\') and friendid = (select id from useinfo where name = \'%4\'))")
                         .arg(fname).arg(mname).arg(mname).arg(fname);
    QSqlQuery sqlque;
    sqlque.exec(sqlsqe);
    if(sqlque.next()){
        return 0;
    }else{
        sqlsqe = QString("select online from useinfo where name = \'%1\'").arg(fname);
        sqlque.exec(sqlsqe);
        if(sqlque.next()){
            int ret = sqlque.value(0).toInt();
            if(ret == 1){
                return 1;   //不是好友，但在线
            }else{
                return 2;   //不是好友，不在线
            }
        }else{
            return 3;   //查无此人
        }
    }
}

bool opDB::handagreeadd(const char *addedName, const char *sourceName)
{
    if(NULL == addedName || NULL == sourceName)
    {
        qDebug() << "handleAddFriendAgree: name is NULL";
        return false;
    }

    int sourceUserId = -1, addedUserId = -1;
    // 查找用户对应id
    addedUserId = getIdByUserName(addedName);
    sourceUserId = getIdByUserName(sourceName);

    QString strQuery = QString("insert into friendInfo values(%1, %2) ").arg(sourceUserId).arg(addedUserId);
    QSqlQuery query;

    qDebug() << "handleAddFriendAgree " << strQuery;

    return query.exec(strQuery);

}

int opDB::getIdByUserName(const char *name)
{
    if(NULL == name)
    {
        return -1;
    }
    QString strQuery = QString("select id from useinfo where name = \'%1\' ").arg(name);
    QSqlQuery query;

    query.exec(strQuery);
    if(query.next())
    {
        return query.value(0).toInt();
    }
    else
    {
        return -1; // 不存在该用户
    }
}

QStringList opDB::handonline()
{
    QStringList ret;
    QString sqlque = QString("select name from useinfo where online = 1");
    QSqlQuery quey;
    quey.exec(sqlque);
    while(quey.next()){
        ret.append(quey.value(0).toString());
    }
    return ret;
}
