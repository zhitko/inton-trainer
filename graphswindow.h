#ifndef GRAPHSWINDOW_H
#define GRAPHSWINDOW_H

#include <QWidget>

#include <mgl2/qmathgl.h>

#define GRAPH_HEIGHT 300
#define GRAPH_K_MAX 300
#define GRAPH_K_INIT 40

namespace Ui {
    class GraphsWindow;
}

class Drawer;
class QScrollArea;
class SoundRecorder;

class GraphsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GraphsWindow(QWidget *parent = 0);
    explicit GraphsWindow(QString path, QWidget *parent = 0);
    ~GraphsWindow();
    void hideZoomControls();
    void drawFile(QString path);

protected:
    Ui::GraphsWindow *ui;
    QScrollArea * scrollArea;
    QMathGL *QMGL;
    Drawer * drawer;
    QString fileName, path;
    QString lastImageFile;
    SoundRecorder * recorder;
    int k_graph, w_graph;
    void initUI();
    virtual Drawer * createNewDrawer(QString path);

signals:
    void changeSig(int by);

    void autoRec();
    void rec();
    void recFinish();

public slots:
    void _autoRec();
    void _rec();

    void startRecord();
    void stopRecord(SoundRecorder *);
    void stopRecord();
    void startAutoRecord();

    void stereo();
    void increase();
    void decrease();
    void increase(int by);
    void decrease(int by);
    void fit();
    void setK(int k);
    void setFitByK();
    void fullFit();
    void saveImage();
    void openImage();
    void playRecord();
};

#endif // GRAPHSWINDOW_H
