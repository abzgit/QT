#include "tcpclient.h"
#include "friedns.h"

#include <QApplication>
#include <openwidget.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Tcpclient w;
    // w.show();
    // friends w;
    // w.show();
    openwidget w;
    w.show();
    return a.exec();
}
