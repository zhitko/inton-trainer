#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

extern "C" {
    #include "./OpenAL/openal_wrapper.h"
    #include "./OpenAL/wavFile.h"
}

#define DATA_PATH "/data"
#define USER_DATA_PATH "/RECORDS/"
#define WAVE_TYPE ".wav"

namespace Ui {
    class MainWindow;
}

class QAction;
class QListWidget;
class QMenu;
class QComboBox;
class QListWidgetItem;

class Drawer;
class SoundRecorder;
class AutoSoundRecorder;
class GraphsWindow;
class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:

    GraphsWindow * showGraph(QString);

    void initUI();

    SoundRecorder *recorder;
    AutoSoundRecorder *autoRecorder;

    QAction *recordingAct;
    QAction *autoRecordingAct;

    void recording(SoundRecorder*);

private:
    Ui::MainWindow *ui;
    SettingsDialog *settingsDialog;

private slots:
    void training();
    void evaluationF0();
    void evaluationI();
    void evaluation(Drawer * drawer);
    void evaluationTest();
    void remove();
    void rename();
    void autoRecording();
    void manualRecording();
    void plotting();
    void compare();
    void recordFinished(SoundRecorder*);
    void updateVolume();
    void playRecord();
    void updateFileList();

    void settingsShow();

private slots:
    void plottingGraph(QListWidgetItem*);
    void trainingGraph(QListWidgetItem*);
    void evaluationGraph(QListWidgetItem* item);
};

#endif // MAINWINDOW_H
