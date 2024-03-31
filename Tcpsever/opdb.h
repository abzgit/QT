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
    void initdb();
    ~opDB();
    bool handregist(const char *name,const char *pwd);
    bool handlogin(const char *name,const char *pwd);
    void handoffline(const char *name);
    int handsearch(const char *name);
    int handadd(const char* fname,const char* mname);
    bool handagreeadd(const char *addedName, const char *sourceName);
    int getIdByUserName(const char* name);

    QStringList handonline();

private:
    QSqlDatabase m_db;

signals:
};

#endif // OPDB_H
