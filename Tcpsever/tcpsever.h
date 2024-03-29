#ifndef TCPSEVER_H
#define TCPSEVER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class TcpSever;
}
QT_END_NAMESPACE

class TcpSever : public QWidget
{
    Q_OBJECT

public:
    TcpSever(QWidget *parent = nullptr);
    ~TcpSever();
    void loadconfig();

private:
    Ui::TcpSever *ui;
    QString m_ip;
    qint16 m_port;
};
#endif // TCPSEVER_H
