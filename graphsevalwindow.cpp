#include "graphsevalwindow.h"

#include <QDebug>
#include <QMessageBox>

#include "drawereval.h"

GraphsEvalWindow::GraphsEvalWindow(QString path, QWidget *parent) :
    GraphsWindow(parent)
{
    this->path = path;
    this->drawFile(path);
}

GraphsEvalWindow::~GraphsEvalWindow()
{

}

Drawer * GraphsEvalWindow::createNewDrawer(QString path)
{
    qDebug() << "GraphsEvalWindow use DrawerEval";
    if(this->drawer == NULL)
    {
        qDebug() << "GraphsEvalWindow new DrawerEval";
        this->drawer = new DrawerEval(this->path);
        this->QMGL->setDraw(this->drawer);
    }
    else{
        qDebug() << "GraphsEvalWindow Proc";
        ((DrawerEval *)this->drawer)->Proc(path);
        this->QMGL->update();
        QMessageBox::information(this, tr("Score"), tr("Your score:\n %1").arg(((DrawerEval *)this->drawer)->result));
    }
    return this->drawer;
}
