#ifndef OPDB_H
#define OPDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>

class opDB : public QObject
{
    Q_OBJECT
public:
    explicit opDB(QObject *parent = nullptr);
    static opDB& getinstance();
    void initdb();  //初始化连接
    ~opDB();
    bool handregist(const char *name,const char *pwd);  //数据库处理注册
    bool handlogin(const char *name,const char *pwd);   //数据库处理登录
    void handoffline(const char *name);     //数据库处理下线
    int handsearch(const char *name);       //数据库处理搜索
    int handadd(const char* fname,const char* mname);   //数据库处理添加好友
    bool handagreeadd(const char *addedName, const char *sourceName);   //数据库处理同意添加好友
    int getIdByUserName(const char* name);  //数据库通过名字找id

    QStringList handonline();   //获取在线用户

private:
    QSqlDatabase m_db;

signals:
};

#endif // OPDB_H
