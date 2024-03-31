#include "tcpclient.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Tcpclient::getinstance().show();
    // friends w;
    // w.show();
    // openwidget w;
    // w.show();
    return a.exec();
}
