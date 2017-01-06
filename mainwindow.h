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
class DPSoundRecorder;
class GraphsWindow;
class SettingsDialog;
class GraphsEvalWindow;

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
    DPSoundRecorder *dpRecorder;

    QAction *recordingAct;
    QAction *autoRecordingAct;
    QAction *dpRecordingAct;

    void recording(SoundRecorder*);

private:
    Ui::MainWindow *ui;
    SettingsDialog *settingsDialog;

private slots:
    void evaluationF0();
    void evaluationF0_Spec();
    void evaluationDP();
    void evaluationSpec();
    void evaluationI();
    void evaluationI_Spec();
    void evaluation(Drawer*);
    void remove();
    void rename();
    void autoRecording();
    void dpRecording();
    void manualRecording();
    void plotting();
    void compare();
    void recordFinished(SoundRecorder*);
    void updateVolume();
    void playRecord();
    void updateFileList();
    void plottingGraph(QListWidgetItem*);

public slots:
    void settingsShow();
    GraphsEvalWindow * evaluation(QString, Drawer*);
    void playRecord(QString);
};

#endif // MAINWINDOW_H
