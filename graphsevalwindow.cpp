#include "graphsevalwindow.h"

#include "drawereval.h"

GraphsEvalWindow::GraphsEvalWindow(QString path, QWidget *parent) :
    GraphsWindow(parent)
{
    this->path = path;
}

Drawer * GraphsEvalWindow::createNewDrawer(QString path)
{
    if(this->drawer) delete drawer;
    return new Drawer(path);
}
