#include "graphsevalwindow.h"
#include "ui_graphswindow.h"

#include <QDebug>
#include <QMessageBox>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>

#include "graphswindow.h"

#include "drawerdp.h"
#include "soundplayer.h"
#include "settingsdialog.h"

#include "xlsxdocument.h"

#include <QSettings>

GraphsEvalWindow::GraphsEvalWindow(QString path, Drawer * drawer, QWidget *parent) :
    GraphsWindow(parent)
{
    this->templatePath = path;
    this->path = path;
    this->drawer = drawer;
    this->QMGL->setDraw(this->drawer);
    this->drawFile(path);
    this->ui->playTemplateBtn->hide();
    this->ui->saveMetrics->hide();
    this->ui->openMetrics->hide();
    connect(this->ui->playTemplateBtn, SIGNAL(clicked()), this, SLOT(playTemplate()));
    connect(this->ui->showUMP, SIGNAL(clicked()), this, SLOT(showUMP()));
    connect(this->ui->saveMetrics, SIGNAL(clicked()), this, SLOT(saveMetrics()));
    connect(this->ui->openMetrics, SIGNAL(clicked()), this, SLOT(openMetrics()));


    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    this->drawer->showUMP = sptk_settings->dp->showPortr;
    if(this->drawer->showUMP)
        this->ui->showUMP->setText("Show original");
    else
        this->ui->showUMP->setText("Show Universal Melodic Portrait (UMP)");
}

GraphsEvalWindow::~GraphsEvalWindow()
{

}

Drawer * GraphsEvalWindow::createNewDrawer(QString path)
{
    this->ui->saveMetrics->show();
    this->ui->playTemplateBtn->show();
    this->drawer->Proc(path);
    this->path = path;

    this->ui->totalC->setText(QString::number(this->drawer->proximity_curve_correlation));
    this->ui->totalI->setText(QString::number(this->drawer->proximity_curve_integral));
    this->ui->totalL->setText(QString::number(this->drawer->proximity_curve_local));
    this->ui->totalA->setText(QString::number(this->drawer->proximity_average));
    this->ui->totalPr->setText(QString::number(this->drawer->proximity_range));

    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    switch (sptk_settings->dp->errorType) {
    case 0:
        this->ui->totalC->setStyleSheet("font-weight: bold");
        this->ui->totalCtitle->setStyleSheet("font-weight: bold");
        break;
    case 1:
        this->ui->totalI->setStyleSheet("font-weight: bold");
        this->ui->totalItitle->setStyleSheet("font-weight: bold");
        break;
    case 2:
        this->ui->totalL->setStyleSheet("font-weight: bold");
        this->ui->totalLtitle->setStyleSheet("font-weight: bold");
        break;
    case 3:
        this->ui->totalA->setStyleSheet("font-weight: bold");
        this->ui->totalAtitle->setStyleSheet("font-weight: bold");
        break;
    default:
        break;
    }

    return this->drawer;
}

void GraphsEvalWindow::playTemplate()
{
    SoundPlayer * player = new SoundPlayer(templatePath);
    player->start();
}

void GraphsEvalWindow::showUMP()
{
    if(this->drawer->showUMP)
        this->ui->showUMP->setText("Show Universal Melodic Portrait (UMP)");
    else
        this->ui->showUMP->setText("Show original");
    this->drawer->showUMP = !this->drawer->showUMP;
    this->QMGL->update();
}

void GraphsEvalWindow::saveMetrics()
{
    qDebug() << "saveMetrics" << LOG_DATA;

    QXlsx::Document xlsx;

    QXlsx::Format format;
    format.setFontBold(true);

    xlsx.setColumnWidth(1, 30);
    xlsx.write(1, 1, "Template", format);
    xlsx.write(2, 1, "Record", format);

    xlsx.write(1, 2, this->templatePath);
    xlsx.write(2, 2, this->path);

    QMap<QString, QVariant> data = this->drawer->getStatisticData();

    int row = 3;
    foreach (QString key, data.keys()) {
        xlsx.write(row, 1, key, format);
        xlsx.write(row, 2, data[key]);
        row++;
    }

    QDir templateDir(this->templatePath);
    QString templateName = templateDir.dirName().remove(".wav", Qt::CaseInsensitive);
    QString recordName = QDir(this->path).dirName().remove(".wav", Qt::CaseInsensitive);
    QString fileName  = templateName + " - " + recordName + ".xlsx";

    templateDir.cdUp();
    this->metricsFilePath = templateDir.absoluteFilePath(fileName);
    qDebug() << "save to" << this->metricsFilePath;

    bool isOk = xlsx.saveAs(this->metricsFilePath);

    if (isOk)
    {
        this->ui->openMetrics->show();
    }
}

void GraphsEvalWindow::openMetrics()
{
    QDesktopServices::openUrl(QUrl("file:///" + this->metricsFilePath, QUrl::TolerantMode));
}

