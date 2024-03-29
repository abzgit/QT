#include "tcpsever.h"
#include "opdb.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    opDB::getinstance().initdb();
    TcpSever w;
    w.show();
    return a.exec();
}
