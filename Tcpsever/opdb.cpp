#include "opdb.h"
#include <QMessageBox>
#include <QDebug>
#include <QResource>
#include <QSqlQuery>
#include <QCryptographicHash>

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

void opDB::initdb()     //初始化连接
{
    m_db.setHostName("localhost");
    m_db.setDatabaseName("D:\\Qt\\Tcpsever\\cloud.db");
    if(m_db.open()){

    }else{
        QMessageBox::critical(NULL,"打开数据库","打开数据库失败");
    }
}

opDB::~opDB()
{
    m_db.close();
}

bool opDB::handregist(const char *name, const char *pwd)    //处理注册
{
    if(name == NULL || pwd == NULL){
        return false;
    }
    QByteArray hash;
    QCryptographicHash hashGenerator(QCryptographicHash::Md5);
    hashGenerator.addData(pwd, strlen(pwd));
    hash = hashGenerator.result();
    char *hashpwd = hash.data();
    QString sqlque = QString("insert into useinfo(name,pwd) values(\'%1\',\'%2\')").arg(name).arg(hashpwd);
    QSqlQuery query;
    return query.exec(sqlque);
}

bool opDB::handlogin(const char *name, const char *pwd)     //处理登录
{
    if(name == NULL || pwd == NULL){
        return false;
    }
    QByteArray hash;
    QCryptographicHash hashGenerator(QCryptographicHash::Md5);
    hashGenerator.addData(pwd, strlen(pwd));
    hash = hashGenerator.result();
    char *hashpwd = hash.data();
    QString sqlque = QString("select * from useinfo where name = \'%1\' and pwd = \'%2\' and online = 0").arg(name).arg(hashpwd);
    QSqlQuery quey;
    quey.exec(sqlque);
    if(quey.next()){
        sqlque = QString("update useinfo set online = 1 where name = \'%1\'").arg(name);
        QSqlQuery quey;
        quey.exec(sqlque);
        return true;
    }else{
        return false;
    }
}

void opDB::handoffline(const char *name)    //数据库处理登录
{
    if(name == NULL){
        qDebug()<<"name is NULL!";
        return;
    }
    QString sqlque = QString("update useinfo set online = 0 where name = \'%1\'").arg(name);
    QSqlQuery quey;
    quey.exec(sqlque);
}

int opDB::handsearch(const char *name)      //数据库处理搜索
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

int opDB::handadd(const char *fname, const char *mname) //数据库处理添加好友
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

bool opDB::handagreeadd(const char *addedName, const char *sourceName)  //数据库处理同意好友
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

QStringList opDB::handflush(const char *name)
{
    QStringList strFriendList;
    strFriendList.clear(); // 清除内容

    if (NULL == name)
    {
        return strFriendList;
    }
    int id = getIdByUserName(name);
    QString que = QString("select name,online from useinfo where id in (select "
                          "friendid from friendinfo where id = %1)").arg(id);
    QSqlQuery sqlop;
    sqlop.exec(que);
    QString online = "online";
    QString offline = "offline";
    while(sqlop.next()){
        if(sqlop.value(1).toInt() == 0){
            que = QString("%1   %2").arg(sqlop.value(0).toString()).arg(offline);
        }else{
            que = QString("%1   %2").arg(sqlop.value(0).toString()).arg(online);
        }
        strFriendList.append(que);
    }
    que = QString("select name,online from useinfo where id in (select "
                  "id from friendinfo where friendid = %1)").arg(id);
    sqlop.exec(que);
    while(sqlop.next()){
        if(sqlop.value(1).toInt() == 0){
            que = QString("%1    %2").arg(sqlop.value(0).toString()).arg(offline);
        }else{
            que = QString("%1    %2").arg(sqlop.value(0).toString()).arg(online);
        }
        strFriendList.append(que);
    }
    return strFriendList; // 返回查询到所有在线用户的姓名
}

bool opDB::handdelfriend(const char *mname, const char *fname)
{
    if(mname == NULL || fname == NULL){
        return false;
    }
    int mid,fid;
    mid = getIdByUserName(mname);
    fid = getIdByUserName(fname);
    QString sqlque = QString("delete from friendinfo where (id = %1 and friendid = %2) or"
                             " (id = %3 and friendid = %4)").arg(mid).arg(fid).arg(fid).arg(mid);
    QSqlQuery quey;
    quey.exec(sqlque);
    return true;
}

bool opDB::isonline(const char *name)
{
    int mid = getIdByUserName(name);
    QString sqlque = QString("select online from useinfo where id = %1").arg(mid);
    QSqlQuery quey;
    quey.exec(sqlque);
    mid = quey.value(0).toInt();
    if(mid == 0){
        return false;
    }
    return true;
}

int opDB::getIdByUserName(const char *name) //通过名字找id
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

QStringList opDB::handonline()  //查询在线用户
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
