#ifndef FRIEDNS_H
#define FRIEDNS_H

#include <QWidget>
#include<QTextEdit>
#include <QListWidget>
#include<QLineEdit>
#include<QPushButton>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include "online.h"

class friends : public QWidget
{
    Q_OBJECT
public:
    explicit friends(QWidget *parent = nullptr);
public slots:
    void showonline();

signals:
private:
    QTextEdit *m_msgte; //显示信息
    QListWidget *m_friendlist;  //好友列表
    QLineEdit *m_inputmag;  //输入框

    QPushButton *m_delfriend;
    QPushButton *m_flushfriend;
    QPushButton *m_onlineuser;
    QPushButton *m_searchuser;
    QPushButton *m_sendmag;
    QPushButton *m_privatechat;
    online *m_online;
};

#endif // FRIEDNS_H
