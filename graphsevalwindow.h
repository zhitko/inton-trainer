#ifndef GRAPHSEVALWINDOW_H
#define GRAPHSEVALWINDOW_H

#include "graphswindow.h"

class GraphsEvalWindow : public GraphsWindow
{
    Q_OBJECT
public:
    explicit GraphsEvalWindow(QString path, QWidget *parent = 0);
    ~GraphsEvalWindow();

protected:
    virtual Drawer * createNewDrawer(QString path);

signals:

public slots:

};

#endif // GRAPHSEVALWINDOW_H
