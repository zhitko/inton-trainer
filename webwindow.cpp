#include "webwindow.h"
#include "mainwindow.h"
#include "ui_webwindow.h"
#include "qdebug.h"

#include "drawer.h"
#include "drawerdp.h"
#include "settingsdialog.h"

#include <QDir>
#include <QDirIterator>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QSettings>

WebWindow::WebWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WebWindow)
{
    ui->setupUi(this);
    this->initWeb();
    this->mainWindow = new MainWindow(this);
}

WebWindow::~WebWindow()
{
    delete ui;
}

void WebWindow::attachObject()
{
    QWebFrame * frame = this->ui->webView->page()->mainFrame();
    frame->addToJavaScriptWindowObject( QString("api"), this );
}

QStringList scanDirItems()
{
    QDir fullDir(QApplication::applicationDirPath() + DATA_PATH_TRAINING);
    QDir dir(QApplication::applicationDirPath() + DATA_PATH);
    QStringList files;
    QString fullPath = fullDir.absolutePath();
    QString path = dir.absolutePath();
    qDebug() << "Search in " << path << LOG_DATA;
    QDirIterator iterator(fullPath, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        iterator.next();
        if (!iterator.fileInfo().isDir()) {
            QString filename = iterator.filePath();
            if (filename.endsWith(WAVE_TYPE))
                files.append(filename.remove(path));
        }
    }
    return files;
}


bool WebWindow::isShowA0()
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
    return sptk_settings->dp->showA0;
}

bool WebWindow::isShowF0()
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
    return sptk_settings->dp->showF0;
}

bool WebWindow::isShowError()
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
    return sptk_settings->dp->showError;
}

bool WebWindow::isShowTime()
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
    return sptk_settings->dp->showTime;
}

void WebWindow::setShowA0(QVariant value)
{
    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
    settings.setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());
    settings.setValue("dp/showA0", value.toBool());
    qDebug() << "setShowA0 " << value.toBool() << LOG_DATA;
}

void WebWindow::setShowF0(QVariant value)
{
    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
    settings.setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());
    settings.setValue("dp/showF0", value.toBool());
    qDebug() << "setShowF0 " << value.toBool() << LOG_DATA;
}

void WebWindow::setShowError(QVariant value)
{
    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
    settings.setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());
    settings.setValue("dp/showError", value.toBool());
    qDebug() << "setShowError " << value.toBool() << LOG_DATA;
}

void WebWindow::setShowTime(QVariant value)
{
    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
    settings.setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());
    settings.setValue("dp/showTime", value.toBool());
    qDebug() << "setShowTime " << value.toBool() << LOG_DATA;
}

QString WebWindow::getFiles()
{
    QStringList files = scanDirItems();
    files.sort();
    QString lastPath = "";

    QJsonArray jResult;
    QJsonObject jSection;
    QJsonArray jList;
    for(int i=0; i<files.size();i++)
    {
        QString file = files.at(i);
        int lastIndex = file.lastIndexOf("/");
        int firstIndex = file.indexOf("/", 1);

        QString filePath = file.left(lastIndex).replace("/", " ");
        QString fileTitle = filePath.mid(firstIndex).replace("/", " ");
        QString fileName = file.mid(lastIndex).replace("/", " ");

        if (lastPath == "") lastPath = fileTitle;

        if (lastPath != fileTitle)
        {
            jSection["title"] = lastPath;
            jSection["files"] = jList;
            jResult.append(jSection);

            QJsonObject jNewSection;
            jSection = jNewSection;
            lastPath = fileTitle;
            QJsonArray jNewList;
            jList = jNewList;
        }

        QJsonObject jItem;
        jItem["title"] = fileName;
        jItem["text"] = fileName;
        jItem["path"] = file;
        jList.append(jItem);
    }
    jSection["title"] = lastPath;
    jSection["files"] = jList;
    jResult.append(jSection);

    QJsonDocument jDoc(jResult);
    return QString(jDoc.toJson(QJsonDocument::Compact));
}

void WebWindow::initWeb()
{
    QWebFrame * frame = this->ui->webView->page()->mainFrame();
    attachObject();
    connect(frame , SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(attachObject()) );

    qDebug() << (QApplication::applicationDirPath() + "/html/index.html") << LOG_DATA;
    this->ui->webView->load(QUrl::fromLocalFile(QApplication::applicationDirPath() + "/html/index.html"));

    ui->webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect(ui->webView, SIGNAL(linkClicked(QUrl)), this, SLOT(linkClickedWebView(QUrl)));
}

void WebWindow::linkClickedWebView(QUrl url) {
    qDebug() << url.path() << LOG_DATA;
    qDebug() << url.fileName() << LOG_DATA;
    qDebug() << url.toString() << LOG_DATA;
    if (url.toString().endsWith(".wav#dp")) {
        this->mainWindow->evaluation(url.path(), new DrawerDP());
    }else if (url.toString().endsWith(".wav#play")) {
        this->mainWindow->playRecord(url.path());
    }else if(url.fileName() == "settings.window"){
        this->mainWindow->settingsShow();
    }else if(url.fileName() == "main.window"){
        this->mainWindow->show();
    }else{
        ui->webView->load(url);
    }
}
