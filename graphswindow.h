#ifndef GRAPHSWINDOW_H
#define GRAPHSWINDOW_H

#include <QWidget>

#include <mgl2/qmathgl.h>

#define GRAPH_HEIGHT 500
#define GRAPH_WIGHT 500

namespace Ui {
    class GraphsWindow;
}

class Drawer;
class QScrollArea;

class GraphsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GraphsWindow(QString path, QWidget *parent = 0);
    ~GraphsWindow();

private:
    Ui::GraphsWindow *ui;
    QScrollArea * scrollArea;
    QMathGL *QMGL;
    Drawer * drawer;
    QString fileName;
    QString lastImageFile;
    void showGraph(QString path);
private slots:
    void rangeChanged();
    void pitchAuto();
    void specAuto();

    void stereo();
    void increase();
    void decrease();
    void fit();
    void saveImage();
    void openImage();
};

#endif // GRAPHSWINDOW_H
