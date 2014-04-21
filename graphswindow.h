#ifndef GRAPHSWINDOW_H
#define GRAPHSWINDOW_H

#include <QWidget>

#include <mgl2/qmathgl.h>

#define GRAPH_HEIGHT 500
#define GRAPH_WIGHT_K 5

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
    void hideZoomControls();

private:
    Ui::GraphsWindow *ui;
    QScrollArea * scrollArea;
    QMathGL *QMGL;
    Drawer * drawer;
    QString fileName;
    QString lastImageFile;
    void showGraph(QString path);

signals:
    void lessSig(int by);
    void moreSig(int by);
    void fitSig();

    void autoRec();
    void rec();

public slots:
    void rangeChanged();
    void pitchAuto();
    void specAuto();

    void _autoRec();
    void _rec();

    void stereo();
    void increase();
    void decrease();
    void increase(int by);
    void decrease(int by);
    void fit();
    void saveImage();
    void openImage();
};

#endif // GRAPHSWINDOW_H
