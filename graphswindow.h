#ifndef GRAPHSWINDOW_H
#define GRAPHSWINDOW_H

#include "defines.h"

#include <QWidget>
#include <QResizeEvent>
#include <QTimer>

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
    void drawFile(QString path);

protected:
    Ui::GraphsWindow *ui;
    QScrollArea * scrollArea;
    QMathGL *QMGL;
    Drawer * drawer;
    QString fileName, path;
    QString lastImageFile;
    SoundRecorder * recorder;
    QTimer * resizeTimer;
    int k_graph, w_graph;
    void initUI();
    virtual Drawer * createNewDrawer(QString path);
    void resizeEvent(QResizeEvent *event) override;

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
    void openFile();

    void fullFit();
    void playRecord();
};

#endif // GRAPHSWINDOW_H
