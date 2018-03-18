#include "webwindow.h"
#include <QApplication>

#include <QTextCodec>

#include <locale.h>

#include <qapplication.h>
#include <stdio.h>
#include <stdlib.h>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QFileInfo>
#include <QPixmap>
#include <QSplashScreen>

#include <QDebug>
#include "defines.h"

static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(0);

void logToFile(const QString message)
{
    QFile file(LOG_FILE_NAME);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        QTextStream stream(&file);
        stream << message << endl;
    }
}

void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QString message = QString("%1: %2").arg(
                QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss "),
                QString(localMsg.constData())
    );
    logToFile(message);

    (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, message);
}

int main(int argc, char *argv[])
{
    setlocale( LC_CTYPE, ".1251" );

    QCoreApplication::setOrganizationName("Zhitko Vladimir");
    QCoreApplication::setOrganizationDomain("vladimir.zhitko.by");
    QCoreApplication::setApplicationName("IntonTrainer");
    QCoreApplication::setApplicationVersion(QString(__DATE__));

    qInstallMessageHandler(messageOutput);

    qDebug() << FULL_APP_VERIOSN;

    QApplication app(argc, argv);

    QPixmap pixmap(":/splash/icons/wave.png");
    QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);
    splash.show();
    app.processEvents();

    splash.showMessage("Loaded modules");

    WebWindow window;
    window.show();
    window.setWindowTitle(FULL_APP_VERIOSN);
    window.setWindowIcon(QIcon(":/icons/icons/inton-50.png"));
    splash.finish(&window);
    return app.exec();
}
