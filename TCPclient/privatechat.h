#ifndef PRIVATECHAT_H
#define PRIVATECHAT_H

#include <QWidget>
#include "protocol.h"
#include "tcpclient.h"

namespace Ui {
class privatechat;
}

class privatechat : public QWidget
{
    Q_OBJECT

public:
    explicit privatechat(QWidget *parent = nullptr);
    ~privatechat();
    void setchatname(QString strname);
    static privatechat &getinsance();
private slots:
    void on_sendmsg_clicked();

private:
    Ui::privatechat *ui;
    QString m_chatname;
    QString m_strlogname;
};

#endif // PRIVATECHAT_H
