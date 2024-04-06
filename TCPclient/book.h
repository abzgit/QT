#pragma once
#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <protocol.h>
#include <QTimer>

class book : public QWidget
{
    Q_OBJECT
public:
    explicit book(QWidget *parent = nullptr);
    void handflushfile(pdu *Pdu);
    void clearmenterdir();
    QString getenterdir();
    void setdownload(bool statu);
    qint64 total;
    qint64 recived;
    bool getstatu();
    QString getsavepath();
    QString getsharename();

signals:

private:
    QListWidget *m_booklist;
    QPushButton *m_return;
    QPushButton *m_createdir;
    QPushButton *m_deldir;
    QPushButton *m_renamedir;
    QPushButton *m_flushdir;
    QPushButton *m_uploadfile;
    QPushButton *m_delfile;
    QPushButton *m_downloadfile;
    QPushButton *m_sharefile;
    QPushButton *movefile;
    QPushButton *selectdesdir;

    QString m_enterdir;
    QString m_uploadfilepath;

    QTimer *m_timer;

    QString m_savepath;
    bool isdownload;

    QString msharefilename;

    QString movefilename;
    QString movefilepath;
    QString desdir;


public slots:
    void creatdir();
    void flushfile();
    void deldir();
    void renamedir();
    void enterdir(const QModelIndex &index);
    void returnpredir();
    void uploadfile();
    void uploadfiledate();
    void delfile();
    void download();
    void sharefile();
    void movefileordir();
    void selectdir();

};

#endif // BOOK_H
