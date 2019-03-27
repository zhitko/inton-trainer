#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QAction>
#include <QKeySequence>
#include <QScrollArea>
#include <QToolBar>
#include <QToolButton>
#include <QComboBox>
#include <QLabel>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QStringList>
#include <QApplication>
#include <QList>
#include <QTimer>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QInputDialog>

#include "soundplayer.h"
#include "recorder/soundrecorder.h"
#include "recorder/autosoundrecorder.h"
#include "recorder/dpsoundrecorder.h"
#include "recorder/dpsoundrecorder.h"

#include "graphswindow.h"
#include "graphsevalwindow.h"
#include "settingsdialog.h"

#include "drawer.h"
#include "drawerdp.h"

extern "C" {
    #include "./sptk/SPTK.h"
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      recorder(NULL),
      autoRecorder(NULL),
      dpRecorder(NULL),
      settingsDialog(NULL)
{
    qDebug() << "MainWindow" << SETTINGS_FILE << LOG_DATA;
    ui->setupUi(this);
    this->initUI();
    this->updateFileList();
    this->settingsDialog = SettingsDialog::getInstance(this);
}

MainWindow::~MainWindow()
{
    qDebug() << "~MainWindow" << SETTINGS_FILE << LOG_DATA;
    if(this->recorder) this->recorder->deleteLater();
    if(this->autoRecorder) this->autoRecorder->deleteLater();
    if(this->dpRecorder) this->dpRecorder->deleteLater();
    delete ui;
}

void MainWindow::initUI()
{
    qDebug() << "" << SETTINGS_FILE << LOG_DATA;
    QToolBar * trainingToolBar = addToolBar(tr("Training"));
    trainingToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    trainingToolBar->setIconSize(QSize(16,16));

    QAction * ratingDpAct = new QAction(tr("Show DP"), this);
    connect(ratingDpAct, SIGNAL(triggered()), this, SLOT(evaluationDP()));
    trainingToolBar->addAction(ratingDpAct);

    // sound actions
    QAction * playAct = new QAction(tr("&Play"), this);
    playAct->setIcon(QIcon(":/icons/icons/speaker-26.png"));
    playAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
    playAct->setStatusTip(tr("Play current audio"));
    connect(playAct, SIGNAL(triggered()), this, SLOT(playRecord()));

    this->recordingAct = new QAction(tr("&Record"), this);
    this->recordingAct->setIcon(QIcon(":/icons/icons/record-26.png"));
    this->recordingAct->setStatusTip(tr("Start/Stop recording audio"));
    connect(this->recordingAct, SIGNAL(triggered()), this, SLOT(manualRecording()));

    this->autoRecordingAct = new QAction(tr("&Auto Record"), this);
    this->autoRecordingAct->setIcon(QIcon(":/icons/icons/voice_recognition_scan-26.png"));
    this->recordingAct->setStatusTip(tr("Start/Stop auto recording audio"));
    connect(this->autoRecordingAct, SIGNAL(triggered()), this, SLOT(autoRecording()));

    this->dpRecordingAct = new QAction(tr("&DP Record"), this);
    this->dpRecordingAct->setIcon(QIcon(":/icons/icons/voice_recognition_scan-26.png"));
    this->recordingAct->setStatusTip(tr("Start/Stop auto recording audio"));
    connect(this->dpRecordingAct, SIGNAL(triggered()), this, SLOT(dpRecording()));

    QToolBar * actionToolBar = addToolBar(tr("Actions"));
    actionToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    actionToolBar->setIconSize(QSize(16,16));
    actionToolBar->addAction(this->recordingAct);
    actionToolBar->addAction(this->autoRecordingAct);
    actionToolBar->addAction(this->dpRecordingAct);
    actionToolBar->addAction(playAct);

    // Files' actions

    QAction * removeAct = new QAction(tr("Remove"), this);
    removeAct->setIcon(QIcon(":/icons/icons/delete-26.png"));
    removeAct->setStatusTip(tr("Remove selected files"));
    connect(removeAct, SIGNAL(triggered()), this, SLOT(remove()));

    QAction * renameAct = new QAction(tr("Rename"), this);
    renameAct->setIcon(QIcon(":/icons/icons/edit-26.png"));
    renameAct->setStatusTip(tr("Rename selected file"));
    connect(renameAct, SIGNAL(triggered()), this, SLOT(rename()));

    QToolBar * fileToolBar = addToolBar(tr("Files Operations"));
    fileToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    fileToolBar->setIconSize(QSize(16,16));
    fileToolBar->addAction(removeAct);
    fileToolBar->addAction(renameAct);

    // Programm settings tool bar
    QAction * settingsAct = new QAction(tr("Settings"), this);
    settingsAct->setIcon(QIcon(":/icons/icons/settings-26.png"));
    settingsAct->setStatusTip(tr("Open settings dialog"));
    connect(settingsAct, SIGNAL(triggered()), this, SLOT(settingsShow()));

    QToolBar * settingsToolBar = addToolBar(tr("Audio Devices"));
    settingsToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    settingsToolBar->setIconSize(QSize(16,16));
    settingsToolBar->addAction(settingsAct);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateVolume()));
    timer->start(200);

    connect( this->ui->filesList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
             this, SLOT(plottingGraph(QListWidgetItem*)) );
}

void MainWindow::autoRecording()
{
    oal_device * currentDevice = this->settingsDialog->getInputDevice();
    if (!currentDevice) return;

    this->raise();
    if(this->autoRecorder == NULL)
    {
        qDebug() << "new SoundRecorder: " << currentDevice->name << LOG_DATA;
        this->autoRecorder = new AutoSoundRecorder(currentDevice, sizeof(short int));
        qDebug() << "is recording: " << this->autoRecorder->isRecording() << LOG_DATA;
        connect(this->autoRecorder, SIGNAL(resultReady(SoundRecorder *)), this, SLOT(recordFinished(SoundRecorder *)));
        this->autoRecordingAct->setIconText(tr("&Auto Recording..."));
        this->recordingAct->setEnabled(false);
    }
    this->recording(this->autoRecorder);
}

void MainWindow::dpRecording()
{
    oal_device * currentDevice = this->settingsDialog->getInputDevice();
    if (!currentDevice) return;

    this->raise();
    if(this->dpRecorder == NULL)
    {
        QString path = QApplication::applicationDirPath() + DATA_PATH;
        bool ok = true;

        QList<QListWidgetItem*> items = ui->filesList->selectedItems();
        if(items.size() > 0)
        {
            path += items.at(0)->text();
        } else {
            QStringList items;
            for(int i=0;i<this->ui->filesList->count();i++)
                items.append(this->ui->filesList->item(i)->text());
            path += QInputDialog::getItem(this, tr("Pattern"), tr("Record"), items, 0, false, &ok);
        }

        if (ok && !path.isEmpty()){
            qDebug() << "Start DP Recorder for " << path << LOG_DATA;
            qDebug() << "new SoundRecorder: " << currentDevice->name << LOG_DATA;
            this->dpRecorder = new DPSoundRecorder(path, currentDevice, sizeof(short int), this);
            qDebug() << "is recording: " << this->dpRecorder->isRecording() << LOG_DATA;
            connect(this->dpRecorder, SIGNAL(resultReady(SoundRecorder *)), this, SLOT(recordFinished(SoundRecorder *)));
            this->dpRecordingAct->setIconText(tr("&DP Recording..."));
            this->recordingAct->setEnabled(false);
        }
    }
    this->recording(this->dpRecorder);
}

void MainWindow::manualRecording()
{
    oal_device * currentDevice = this->settingsDialog->getInputDevice();
    if (!currentDevice) return;

    this->setFocus();
    if(this->recorder == NULL)
    {
        qDebug() << "new SoundRecorder: " << currentDevice->name << LOG_DATA;
        this->recorder = new SoundRecorder(currentDevice, sizeof(short int), this);
        qDebug() << "is recording: " << this->recorder->isRecording() << LOG_DATA;
        connect(this->recorder, SIGNAL(resultReady(SoundRecorder *)), this, SLOT(recordFinished(SoundRecorder *)));
        this->recordingAct->setIconText(tr("&Recording..."));
        this->autoRecordingAct->setEnabled(false);
    }
    this->recording(this->recorder);
}

void MainWindow::recording(SoundRecorder * recorder)
{
    if(recorder->isRecording())
    {
        qDebug() << "stop recording" << LOG_DATA;
        recorder->stopRecording();
    }else{
        qDebug() << "start recording" << LOG_DATA;
        recorder->startRecording();
    }
}

void MainWindow::recordFinished(SoundRecorder * recorder)
{
    qDebug() << "recordFinished" << LOG_DATA;

    this->recordingAct->setIconText(tr("&Record"));
    this->recordingAct->setEnabled(true);
    this->autoRecordingAct->setIconText(tr("&Auto Record"));
    this->autoRecordingAct->setEnabled(true);
    this->dpRecordingAct->setIconText(tr("&DP Record"));
    this->dpRecordingAct->setEnabled(true);

    char *data;
    int size = recorder->getData((void**) &data);
    qDebug() << "result size " << size << LOG_DATA;

    QDateTime dateTime = QDateTime::currentDateTime();
    QString path = USER_DATA_PATH + dateTime.toString("dd.MM.yyyy hh.mm.ss.zzz");

    path = QApplication::applicationDirPath() + DATA_PATH + path + WAVE_TYPE;
    qDebug() << "write wave to: " << path << LOG_DATA;

    WaveFile *waveFile = makeWaveFileFromRawData(
                (char *)data,
                size,
                NUMBER_OF_CHANNELS,
                RECORD_FREQ,
                SIGNIFICANT_BITS_PER_SAMPLE,
                NULL,
                NULL,
                NULL,
                NULL
    );
    saveWaveFile(waveFile, path.toLocal8Bit().data());
    waveCloseFile(waveFile);

    recorder->deleteLater();
    this->recorder = NULL;
    this->autoRecorder = NULL;
    this->dpRecorder = NULL;
    MainWindow::cleanRecordFiles();
    this->updateFileList();

    if(SettingsDialog::getInstance()->getMathGLSettings()->autoOpen)
    {
        GraphsWindow * graph = this->showGraph(path);
        graph->show();
        graph->fullFit();
    }
}

QStringList scanDirIter(QDir dir)
{
    QStringList files;
    QString path = dir.absolutePath();
    qDebug() << "Search in " << path << LOG_DATA;
    QDirIterator iterator(path, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        iterator.next();
        if (!iterator.fileInfo().isDir()) {
            QString filename = iterator.filePath();
            if (filename.endsWith(WAVE_TYPE))
                files.append(filename.remove(path));
        }
    }
    return files;
}

void MainWindow::updateFileList()
{
    ui->filesList->clear();
    QDir directory(QApplication::applicationDirPath() + DATA_PATH);
    QStringList files = scanDirIter(directory);
    for(int i=0; i<files.size();i++)
    {
        ui->filesList->addItem(files.at(i));
    }
}

void MainWindow::cleanRecordFiles()
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    QString path = QApplication::applicationDirPath() + DATA_PATH + USER_DATA_PATH;
    QDir directory(path);

    QStringList files = scanDirIter(directory);
    files.sort();

    qDebug() << "Files list" << files;
    int count = files.size();
    int i=sptk_settings->dp->recordingMaxFiles - 1;
    for(i; i<count; i++)
    {
        QFile(path + files.at(i)).remove();
        qDebug() << "delete" << files.at(i);
    }
}

void MainWindow::compare()
{
    QStringList items;
    for(int i=0;i<this->ui->filesList->count();i++)
        items.append(this->ui->filesList->item(i)->text());

    QListWidgetItem* item = ui->filesList->currentItem();
    QString path = QApplication::applicationDirPath() + DATA_PATH;
    QString path1 = path + item->text();

    items.removeAll(item->text());
    bool ok;
    QString path2 = path + QInputDialog::getItem(this, tr("Compare with"), tr("Record"), items, 0, false, &ok);
    if (ok && !path2.isEmpty()){
        qDebug() << "Draw graphs for " << path1 << " and " << path2 << LOG_DATA;
        QWidget * window = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(window);
        GraphsWindow * first = this->showGraph(path1);
        GraphsWindow * second = this->showGraph(path2);

        connect(first, SIGNAL(changeSig(int)), second, SLOT(setK(int)));
        connect(second, SIGNAL(changeSig(int)), first, SLOT(setK(int)));

        layout->addWidget(first);
        layout->addWidget(second);
        window->show();
        first->fullFit();
        second->fullFit();
    }
}
void MainWindow::evaluationDP()
{
    evaluation(new DrawerDP());
}

void MainWindow::evaluation(Drawer * drawer)
{
    QString path = QApplication::applicationDirPath() + DATA_PATH;

    QList<QListWidgetItem*> items = ui->filesList->selectedItems();
    if(items.size() > 0)
    {
        QString file = items.at(0)->text();

        GraphsEvalWindow * window = this->evaluation(file, drawer);

        if(items.size() > 1)
        {
            QString file2 = path + items.at(1)->text();
            window->drawFile(file2);
        }
    }
}

GraphsEvalWindow * MainWindow::evaluation(QString filePath, Drawer * drawer)
{
    QString path = QApplication::applicationDirPath() + DATA_PATH;

    QString file = path + filePath;
    qDebug() << "Draw graphs for evaluation " << file << LOG_DATA;
    GraphsEvalWindow * window = new GraphsEvalWindow(file, drawer);

    connect(window, SIGNAL(autoRec()), this, SLOT(autoRecording()));
    connect(window, SIGNAL(rec()), this, SLOT(manualRecording()));

    connect(window, SIGNAL(recFinish()), this, SLOT(updateFileList()));

    window->show();
    window->fullFit();
    return window;
}

void MainWindow::plotting()
{
    QString path = QApplication::applicationDirPath() + DATA_PATH;
    QList<QListWidgetItem*> items = ui->filesList->selectedItems();
    if(items.size() > 0)
    {
        QString file = path + items.at(0)->text();
        qDebug() << "Draw graphs for " << path << LOG_DATA;
        GraphsWindow * graph = this->showGraph(file);
        graph->show();
        graph->fullFit();
    }
}

void MainWindow::plottingGraph(QListWidgetItem*)
{
    this->plotting();
}

GraphsWindow * MainWindow::showGraph(QString path)
{
    GraphsWindow * graph = new GraphsWindow(path);
    connect(graph, SIGNAL(autoRec()), this, SLOT(autoRecording()));
    connect(graph, SIGNAL(rec()), this, SLOT(manualRecording()));
    return graph;
}

void MainWindow::updateVolume()
{
    if(this->recorder != NULL && this->recorder->isRecording())
    {
        long int avg = this->recorder->getVolumeLevel();
        long int max = this->recorder->getMaxVolumeLevel();
        this->ui->volumeBar->setMaximum(max);
        this->ui->volumeBar->setValue(avg);
    }
}

void MainWindow::remove()
{
    int ret = QMessageBox::warning(this, tr("Delete"),
                                   tr("Are you sure you want to delete the selected files?"),
                                   QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                   QMessageBox::No);
    if(ret == QMessageBox::Yes)
    {
        qDebug() << "removing files" << LOG_DATA;
        QString path = QApplication::applicationDirPath() + DATA_PATH;
        QList<QListWidgetItem*> items = ui->filesList->selectedItems();
        for(int i=0; i<items.size(); i++)
        {
            qDebug() << "delete file " << items.at(i)->text() << LOG_DATA;
            QString file = path + items.at(i)->text();
            QFile(file).remove();
        }
        this->updateFileList();
    }
}

void MainWindow::rename()
{
    QList<QListWidgetItem*> items = ui->filesList->selectedItems();
    for(int i=0; i<items.size(); i++)
    {
        QString orig_name, name = orig_name = items.at(i)->text();
        QString old_name = name.remove(name.lastIndexOf('.'), name.length());
        bool ok;
        QString new_name = QInputDialog::getText(this, tr("Rename"),
                                             tr("New file name:"),
                                             QLineEdit::Normal,
                                             old_name, &ok);
        if (ok && !new_name.isEmpty()){
            QString path = QApplication::applicationDirPath() + DATA_PATH;
            QFile(path + orig_name).rename(QDir(path).absoluteFilePath(new_name + WAVE_TYPE));
        }
    }
    this->updateFileList();
}

void MainWindow::settingsShow()
{
    if(this->settingsDialog)
    {
        this->settingsDialog->loadSettings();
        this->settingsDialog->show();
    }
}

void MainWindow::playRecord()
{
    QList<QListWidgetItem*> items = ui->filesList->selectedItems();
    for(int i=0; i<items.size(); i++)
    {
        qDebug() << "play file " << items.at(i)->text() << LOG_DATA;
        this->playRecord(items.at(i)->text());
    }
}

void MainWindow::playRecord(QString filePath)
{
    QString path = QApplication::applicationDirPath() + DATA_PATH;
    QString file = path +filePath;
    SoundPlayer * player = new SoundPlayer(file);
    player->start();
}
