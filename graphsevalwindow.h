#ifndef GRAPHSEVALWINDOW_H
#define GRAPHSEVALWINDOW_H

#include <QWidget>

#include "graphswindow.h"

//class GraphsWindow;
//class Drawer;
//class QScrollArea;
//class SoundRecorder;

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

};

#endif // GRAPHSEVALWINDOW_H
