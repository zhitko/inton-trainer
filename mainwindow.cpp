#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QAction>
#include <QKeySequence>
#include <QScrollArea>
#include <QToolBar>
#include <QComboBox>
#include <QLabel>
#include <QDateTime>
#include <QDir>
#include <QStringList>
#include <QApplication>
#include <QList>
#include <QTimer>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QInputDialog>

#include "soundrecorder.h"
#include "autosoundrecorder.h"

#include "graphswindow.h"
#include "settingsdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      recorder(NULL),
      autoRecorder(NULL),
      settingsDialog(NULL)
{
    ui->setupUi(this);
    this->initUI();
    this->updateFileList();
//    this->settingsDialog = new SettingsDialog(this);
    this->settingsDialog = SettingsDialog::getInstance(this);
}

MainWindow::~MainWindow()
{    
    delete ui;
}

void MainWindow::initUI()
{
    // Recording actions
    this->recordingAct = new QAction(tr("&Record"), this);
    this->recordingAct->setIcon(QIcon(":/icons/icons/record-26.png"));
    this->recordingAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    this->recordingAct->setStatusTip(tr("Start/Stop recording audio"));
    connect(this->recordingAct, SIGNAL(triggered()), this, SLOT(manualRecording()));

    this->autoRecordingAct = new QAction(tr("&Auto Record"), this);
    this->autoRecordingAct->setIcon(QIcon(":/icons/icons/voice_recognition_scan-26.png"));
    this->autoRecordingAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
    this->recordingAct->setStatusTip(tr("Start/Stop auto recording audio"));
    connect(this->autoRecordingAct, SIGNAL(triggered()), this, SLOT(autoRecording()));

    this->actionToolBar = addToolBar(tr("Actions"));
    this->actionToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    this->actionToolBar->setIconSize(QSize(16,16));
    this->actionToolBar->addAction(this->recordingAct);
    this->actionToolBar->addAction(this->autoRecordingAct);

    // Files' actions
    this->plottingAct = new QAction(tr("Show &Graphs"), this);
    this->plottingAct->setIcon(QIcon(":/icons/icons/electrical_threshold-26.png"));
    this->plottingAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
    this->plottingAct->setStatusTip(tr("Show graphs"));
    connect(this->plottingAct, SIGNAL(triggered()), this, SLOT(plotting()));

    this->compareAct = new QAction(tr("Compare"), this);
    this->compareAct->setIcon(QIcon(":/icons/icons/compare-26.png"));
    this->compareAct->setStatusTip(tr("Show compare graphs"));
    connect(this->compareAct, SIGNAL(triggered()), this, SLOT(compare()));

    this->removeAct = new QAction(tr("Remove"), this);
    this->removeAct->setIcon(QIcon(":/icons/icons/delete-26.png"));
    this->removeAct->setStatusTip(tr("Remove selected files"));
    connect(this->removeAct, SIGNAL(triggered()), this, SLOT(remove()));

    this->renameAct = new QAction(tr("Rename"), this);
    this->renameAct->setIcon(QIcon(":/icons/icons/edit-26.png"));
    this->renameAct->setStatusTip(tr("Rename selected file"));
    connect(this->renameAct, SIGNAL(triggered()), this, SLOT(rename()));

    this->fileToolBar = addToolBar(tr("Files Operations"));
    this->fileToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    this->fileToolBar->setIconSize(QSize(16,16));
    this->fileToolBar->addAction(this->plottingAct);
    this->fileToolBar->addAction(this->compareAct);
    this->fileToolBar->addAction(this->removeAct);
    this->fileToolBar->addAction(this->renameAct);

    // Programm settings tool bar
    this->settingsAct = new QAction(tr("Settings"), this);
    this->settingsAct->setIcon(QIcon(":/icons/icons/settings-26.png"));
    this->settingsAct->setStatusTip(tr("Open settings dialog"));
    connect(this->settingsAct, SIGNAL(triggered()), this, SLOT(settingsShow()));

    this->settingsToolBar = addToolBar(tr("Audio Devices"));
    this->settingsToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    this->settingsToolBar->setIconSize(QSize(16,16));
    this->settingsToolBar->addAction(this->settingsAct);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateVolume()));
    timer->start(200);

    connect( this->ui->filesList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(openGraph(QListWidgetItem*)) );
}

void MainWindow::autoRecording()
{
    this->raise();
    if(this->autoRecorder == NULL)
    {
        oal_device * currentDevice = this->settingsDialog->getInputDevice();
        qDebug() << "new SoundRecorder: " << currentDevice->name;
        this->autoRecorder = new AutoSoundRecorder(currentDevice, sizeof(short int), this);
        qDebug() << "is recording: " << this->autoRecorder->isRecording();
        connect(this->autoRecorder, SIGNAL(resultReady(SoundRecorder *)), this, SLOT(recordFinished(SoundRecorder *)));
        this->autoRecordingAct->setIconText(tr("&Auto Recording..."));
        this->recordingAct->setEnabled(false);
    }
    this->recording(this->autoRecorder);
}

void MainWindow::manualRecording()
{
    this->setFocus();
    if(this->recorder == NULL)
    {
        oal_device * currentDevice = this->settingsDialog->getInputDevice();
        qDebug() << "new SoundRecorder: " << currentDevice->name;
        this->recorder = new SoundRecorder(currentDevice, sizeof(short int), this);
        qDebug() << "is recording: " << this->recorder->isRecording();
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
        qDebug() << "stop recording";
        recorder->stopRecording();
    }else{
        qDebug() << "start recording";
        recorder->startRecording();
    }
}

void MainWindow::recordFinished(SoundRecorder * recorder)
{
    qDebug() << "recordFinished";

    this->recordingAct->setIconText(tr("&Record"));
    this->recordingAct->setEnabled(true);
    this->autoRecordingAct->setIconText(tr("&Auto Record"));
    this->autoRecordingAct->setEnabled(true);

    char *data;
    int size = recorder->getData((void**) &data);
    qDebug() << "result size " << size;

    QDateTime dateTime = QDateTime::currentDateTime();
    QString path = dateTime.toString("dd.MM.yyyy hh:mm:ss.zzz");

    path = QApplication::applicationDirPath() + DATA_PATH + path + WAVE_TYPE;
    qDebug() << "write wave to: " << path;
    WaveHeader *headerChunk = makeWaveHeader();
    FormatChunk *formatChunk = makeFormatChunk(1, 8000, 16);
    DataChunk *dataChunk = makeDataChunk(size, (char *)data);
    WaveFile *waveFile = makeWaveFile(headerChunk, formatChunk, dataChunk);
    saveWaveFile(waveFile, path.toLocal8Bit().data());
    waveCloseFile(waveFile);

    delete recorder;
    this->recorder = NULL;
    this->autoRecorder = NULL;
    this->updateFileList();
}

void MainWindow::updateFileList()
{
    ui->filesList->clear();
    QStringList nameFilter(QString("*") + WAVE_TYPE);
    QDir directory(QApplication::applicationDirPath() + DATA_PATH);
    QStringList files = directory.entryList(nameFilter);
    for(int i=0; i<files.size();i++)
    {
        ui->filesList->addItem(files.at(i));
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
        qDebug() << "Draw graphs for " << path1 << " and " << path2;
        QWidget * window = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(window);
        GraphsWindow * first = this->showGraph(path1);
        GraphsWindow * second = this->showGraph(path2);
//        second->hideZoomControls();
//        connect(first, SIGNAL(fitSig()), second, SLOT(fit()));
//        connect(first, SIGNAL(lessSig(int)), second, SLOT(decrease(int)));
//        connect(first, SIGNAL(moreSig(int)), second, SLOT(increase(int)));

        connect(first, SIGNAL(autoRec()), this, SLOT(autoRecording()));
        connect(first, SIGNAL(rec()), this, SLOT(manualRecording()));

        layout->addWidget(first);
        layout->addWidget(second);
        window->show();
    }
}

void MainWindow::plotting()
{
    emit ui->filesList->doubleClicked(ui->filesList->currentIndex());
}

void MainWindow::openGraph(QListWidgetItem* item)
{
    if(!item) return;
    QString path = QApplication::applicationDirPath() + DATA_PATH;
    path += item->text();
    qDebug() << "Draw graphs for " << path;
    GraphsWindow * graph = this->showGraph(path);

    connect(graph, SIGNAL(autoRec()), this, SLOT(autoRecording()));
    connect(graph, SIGNAL(rec()), this, SLOT(manualRecording()));

    graph->show();
}

GraphsWindow * MainWindow::showGraph(QString path)
{
    return new GraphsWindow(path);
}

void MainWindow::updateVolume()
{
    if(this->recorder != NULL && this->recorder->isRecording())
    {
        long int avg = this->recorder->getVolumeLevel();
        long int max = this->recorder->getMaxVolumeLevel();
//        qDebug() << "Volume is " << avg << " of max " << max;
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
        qDebug() << "removing files";
        QString path = QApplication::applicationDirPath() + DATA_PATH;
        QList<QListWidgetItem*> items = ui->filesList->selectedItems();
        for(int i=0; i<items.size(); i++)
        {
            qDebug() << "delete file " << items.at(i)->text();
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
            QFile(path + orig_name).rename(path + new_name + WAVE_TYPE);
        }
    }
    this->updateFileList();
}

void MainWindow::settingsShow()
{
    if(this->settingsDialog)
        this->settingsDialog->show();
}
