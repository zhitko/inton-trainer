#include "webwindow.h"
#include "mainwindow.h"
#include "ui_webwindow.h"
#include "qdebug.h"

#include "drawer.h"
#include "drawerevalpitch.h"
#include "drawerevalpitchbyspectr.h"
#include "drawerevalenergy.h"
#include "drawerevalenergybyspectr.h"
#include "drawerevalspectr.h"

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

void WebWindow::initWeb()
{
    qDebug() << (QApplication::applicationDirPath() + "/html/index.html");
    this->ui->webView->load(QUrl::fromLocalFile(QApplication::applicationDirPath() + "/html/index.html"));

    ui->webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect(ui->webView, SIGNAL(linkClicked(QUrl)), this, SLOT(linkClickedWebView(QUrl)));

}

void WebWindow::linkClickedWebView(QUrl url) {
    qDebug() << url.path();
    qDebug() << url.fileName();
    qDebug() << url.toString();
    if (url.toString().endsWith(".wav#pitch"))
    {
        qDebug() << url.path();
        this->mainWindow->evaluation(url.path(), new DrawerEvalPitchBySpectr());
    }else if (url.toString().endsWith(".wav#energy"))
    {
        qDebug() << url.path();
        this->mainWindow->evaluation(url.path(), new DrawerEvalEnergyBySpectr());
    }else if(url.fileName() == "settings.window"){
        this->mainWindow->settingsShow();
    }else if(url.fileName() == "main.window"){
        this->mainWindow->show();
    }else{
        ui->webView->load(url);
    }
}
