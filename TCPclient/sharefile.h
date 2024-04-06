#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QScrollArea>
#include <QCheckBox>
#include <QListWidget>

class sharefile : public QWidget
{
    Q_OBJECT
public:
    explicit sharefile(QWidget *parent = nullptr);
    static sharefile& getinstance();
    void updatefriend(QListWidget *pfriendlist);
signals:
private:
    QPushButton *allselect;
    QPushButton *allcancle;
    QPushButton *ok;
    QPushButton *cancle;

    QScrollArea *m_PSA;
    QWidget *m_pFriendW;
    QButtonGroup *mbuttongroup;

    QVBoxLayout *mfriendwidget;

public slots:
    void cancleselect();
    void selectall();
    void choseok();
    void chosecancle();

};

#endif // SHAREFILE_H
