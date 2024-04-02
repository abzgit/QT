#pragma once
#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

class book : public QWidget
{
    Q_OBJECT
public:
    explicit book(QWidget *parent = nullptr);

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
};

#endif // BOOK_H
