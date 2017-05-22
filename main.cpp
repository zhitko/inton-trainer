#include "webwindow.h"
#include <QApplication>

#include <QTextCodec>

#include <locale.h>

int main(int argc, char *argv[])
{
    setlocale( LC_CTYPE, ".1251" );
    QCoreApplication::setOrganizationName("Zhitko Vladimir");
    QCoreApplication::setOrganizationDomain("vladimir.zhitko.by");
    QCoreApplication::setApplicationName("IntonTrainer");

    QApplication a(argc, argv);
    WebWindow w;
    w.show();

    return a.exec();
}
