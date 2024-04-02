#include "book.h"

book::book(QWidget *parent)
    : QWidget{parent}
{
    m_booklist = new QListWidget;
    m_return = new QPushButton("返回");
    m_createdir = new QPushButton("创建文件夹");
    m_deldir = new QPushButton("删除文件夹");
    m_renamedir = new QPushButton("重命名");
    m_flushdir = new QPushButton("刷新文件夹");

    QVBoxLayout *pdir = new QVBoxLayout;
    pdir->addWidget(m_return);
    pdir->addWidget(m_createdir);
    pdir->addWidget(m_deldir);
    pdir->addWidget(m_renamedir);
    pdir->addWidget(m_flushdir);

    m_uploadfile = new QPushButton("上传文件");
    m_delfile = new QPushButton("删除文件");
    m_downloadfile = new QPushButton("下载文件");
    m_sharefile = new QPushButton("分享文件");
    QVBoxLayout *pfile = new QVBoxLayout;
    pfile->addWidget(m_uploadfile);
    pfile->addWidget(m_delfile);
    pfile->addWidget(m_downloadfile);
    pfile->addWidget(m_sharefile);

    QHBoxLayout *pmain = new QHBoxLayout;
    pmain->addWidget(m_booklist);
    pmain->addLayout(pdir);
    pmain->addLayout(pfile);

    setLayout(pmain);
}
