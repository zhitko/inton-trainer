#include "graphsevalwindow.h"
#include "ui_graphswindow.h"

#include <QDebug>
#include <QMessageBox>

#include "graphswindow.h"

#include "drawerevalpitch.h"
#include "soundplayer.h"

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

