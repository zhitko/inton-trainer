#ifndef GRAPHSEVALWINDOW_H
#define GRAPHSEVALWINDOW_H

#include "defines.h"

#include <QWidget>

#include "graphswindow.h"

class GraphsEvalWindow : public GraphsWindow
{
    Q_OBJECT
public:
    explicit GraphsEvalWindow(QString path, Drawer * drawer, QWidget *parent = 0);
    ~GraphsEvalWindow();

protected:
    virtual Drawer * createNewDrawer(QString path);
    QString templatePath;

signals:

public slots:
    void playTemplate();
    void showUMP();

};

#endif // GRAPHSEVALWINDOW_H
