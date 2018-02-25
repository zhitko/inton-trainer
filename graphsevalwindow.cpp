#include "graphsevalwindow.h"
#include "ui_graphswindow.h"

#include <QDebug>
#include <QMessageBox>

#include "graphswindow.h"

#include "drawerdp.h"
#include "soundplayer.h"
#include "settingsdialog.h"

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
    connect(this->ui->playTemplateBtn, SIGNAL(clicked()), this, SLOT(playTemplate()));
    connect(this->ui->showUMP, SIGNAL(clicked()), this, SLOT(showUMP()));


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
    this->ui->playTemplateBtn->show();
    this->drawer->Proc(path);

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

