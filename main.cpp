#include "mainwindow.h"
#include <QApplication>

#include <QTextCodec>

int main(int argc, char *argv[])
{
//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1251"));

    QCoreApplication::setOrganizationName("Zhitko");
    QCoreApplication::setOrganizationDomain("vladimir.zhitko.by");
    QCoreApplication::setApplicationName("Voice Analyzer");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
