#include "graphsevalwindow.h"
#include "ui_graphswindow.h"

#include <QDebug>
#include <QMessageBox>

#include "graphswindow.h"

#include "drawerevalpitch.h"
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
        this->ui->showUMP->setText("Show UMP");
}

GraphsEvalWindow::~GraphsEvalWindow()
{

}

Drawer * GraphsEvalWindow::createNewDrawer(QString path)
{
    this->ui->playTemplateBtn->show();
    this->drawer->Proc(path);
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
        this->ui->showUMP->setText("Show UMP");
    else
        this->ui->showUMP->setText("Show original");
    this->drawer->showUMP = !this->drawer->showUMP;
    this->QMGL->update();
}

