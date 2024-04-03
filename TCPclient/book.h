#pragma once
#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <protocol.h>

class book : public QWidget
{
    Q_OBJECT
public:
    explicit book(QWidget *parent = nullptr);
    void handflushfile(pdu *Pdu);
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

public slots:
    void creatdir();
    void flushfile();
};

#endif // BOOK_H
