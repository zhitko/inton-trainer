#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

extern "C" {
    #include "./OpenAL/openal_wrapper.h"
    #include "./OpenAL/wavFile.h"
}

#define DATA_PATH "/data/"
#define WAVE_TYPE ".wav"

namespace Ui {
    class MainWindow;
}

class QAction;
class QListWidget;
class QMenu;
class QComboBox;
class QListWidgetItem;

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

    QToolBar *settingsToolBar;
    QToolBar *actionToolBar;
    QToolBar *fileToolBar;

    QAction *recordingAct;
    QAction *autoRecordingAct;

    QAction *plottingAct;
    QAction *compareAct;

    QAction *removeAct;
    QAction *renameAct;
    QAction *settingsAct;

    void updateFileList();
    void recording(SoundRecorder*);

private:
    Ui::MainWindow *ui;
    SettingsDialog *settingsDialog;

private slots:
    void remove();
    void rename();
    void autoRecording();
    void manualRecording();
    void plotting();
    void compare();
    void recordFinished(SoundRecorder*);
    void updateVolume();

    void settingsShow();

private slots:
    void openGraph(QListWidgetItem*);
};

#endif // MAINWINDOW_H
