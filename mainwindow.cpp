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
#include "soundrecorder.h"
#include "autosoundrecorder.h"

#include "graphswindow.h"
#include "graphsevalwindow.h"
#include "settingsdialog.h"

#include "drawer.h"
#include "drawerevalpitch.h"
#include "drawerevalenergy.h"

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
    this->settingsDialog = SettingsDialog::getInstance(this);
}

MainWindow::~MainWindow()
{    
    delete ui;
}

void MainWindow::initUI()
{
    // trainig tool bar
    QAction * triningAct = new QAction(tr("&Training"), this);
    triningAct->setIcon(QIcon(":/icons/icons/guru-26.png"));
    triningAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
    triningAct->setStatusTip(tr("Start manual training"));
    connect(triningAct, SIGNAL(triggered()), this, SLOT(training()));

    QToolButton * ratingButton = new QToolButton(this);
    ratingButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ratingButton->setText(tr("&Evaluation"));
    ratingButton->setIcon(QIcon(":/icons/icons/trophy-26.png"));
    ratingButton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    ratingButton->setStatusTip(tr("Start manual training"));
    QMenu * menu = new QMenu(this);
    QAction * ratingF0Act = new QAction(tr("Show F0"), this);
    connect(ratingF0Act, SIGNAL(triggered()), this, SLOT(evaluationF0()));
    QAction * ratingIAct = new QAction(tr("Show Energy"), this);
    connect(ratingIAct, SIGNAL(triggered()), this, SLOT(evaluationI()));
    menu->addAction(ratingF0Act);
    menu->addAction(ratingIAct);
    ratingButton->setMenu(menu);
    ratingButton->setPopupMode(QToolButton::InstantPopup);

    QToolBar * trainingToolBar = addToolBar(tr("Training"));
    trainingToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    trainingToolBar->setIconSize(QSize(16,16));
    trainingToolBar->addAction(triningAct);
    trainingToolBar->addWidget(ratingButton);

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

    QToolBar * actionToolBar = addToolBar(tr("Actions"));
    actionToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    actionToolBar->setIconSize(QSize(16,16));
    actionToolBar->addAction(this->recordingAct);
    actionToolBar->addAction(this->autoRecordingAct);
    actionToolBar->addAction(playAct);

    // Files' actions
    QAction * plottingAct = new QAction(tr("Show &Graphs"), this);
    plottingAct->setIcon(QIcon(":/icons/icons/electrical_threshold-26.png"));
    plottingAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
    plottingAct->setStatusTip(tr("Show graphs"));
    connect(plottingAct, SIGNAL(triggered()), this, SLOT(plotting()));

    QAction * compareAct = new QAction(tr("Compare"), this);
    compareAct->setIcon(QIcon(":/icons/icons/compare-26.png"));
    compareAct->setStatusTip(tr("Show compare graphs"));
    connect(compareAct, SIGNAL(triggered()), this, SLOT(compare()));

    QAction * removeAct = new QAction(tr("Remove"), this);
    removeAct->setIcon(QIcon(":/icons/icons/delete-26.png"));
    removeAct->setStatusTip(tr("Remove selected files"));
    connect(removeAct, SIGNAL(triggered()), this, SLOT(remove()));

    QAction * renameAct = new QAction(tr("Rename"), this);
    renameAct->setIcon(QIcon(":/icons/icons/edit-26.png"));
    renameAct->setStatusTip(tr("Rename selected file"));
    connect(renameAct, SIGNAL(triggered()), this, SLOT(rename()));

    addToolBarBreak();
    QToolBar * fileToolBar = addToolBar(tr("Files Operations"));
    fileToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    fileToolBar->setIconSize(QSize(16,16));
    fileToolBar->addAction(plottingAct);
    fileToolBar->addAction(compareAct);
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
             this, SLOT(evaluationGraph(QListWidgetItem*)) );
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
    QString path = USER_DATA_PATH + dateTime.toString("dd.MM.yyyy hh.mm.ss.zzz");

    path = QApplication::applicationDirPath() + DATA_PATH + path + WAVE_TYPE;
    qDebug() << "write wave to: " << path;

    WaveFile *waveFile = makeWaveFileFromData((char *)data, size, 1, 8000, 16);
    saveWaveFile(waveFile, path.toLocal8Bit().data());
    waveCloseFile(waveFile);

    recorder->deleteLater();
    this->recorder = NULL;
    this->autoRecorder = NULL;
    this->updateFileList();

    if(SettingsDialog::getInstance()->getMathGLSettings()->autoOpen)
        this->showGraph(path)->show();
}

QStringList scanDirIter(QDir dir)
{
    QStringList files;
    QString path = dir.absolutePath();
    qDebug() << "Search in " << path;
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

        connect(first, SIGNAL(changeSig(int)), second, SLOT(setK(int)));
        connect(second, SIGNAL(changeSig(int)), first, SLOT(setK(int)));

        layout->addWidget(first);
        layout->addWidget(second);
        window->show();
    }
}

void MainWindow::trainingGraph(QListWidgetItem* item)
{
    this->training();
}

void MainWindow::training()
{
    QString path = QApplication::applicationDirPath() + DATA_PATH;
    QList<QListWidgetItem*> items = ui->filesList->selectedItems();
    if(items.size() > 0)
    {
        QString file = path + items.at(0)->text();
        qDebug() << "Draw graphs for training " << file;
        QWidget * window = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(window);
        GraphsWindow * first = this->showGraph(file);
        GraphsWindow * second = new GraphsWindow();

        connect(first, SIGNAL(changeSig(int)), second, SLOT(setK(int)));
        connect(second, SIGNAL(changeSig(int)), first, SLOT(setK(int)));

        connect(second, SIGNAL(recFinish()), this, SLOT(updateFileList()));

        connect(second, SIGNAL(autoRec()), this, SLOT(autoRecording()));
        connect(second, SIGNAL(rec()), this, SLOT(manualRecording()));

        layout->addWidget(first);
        layout->addWidget(second);
        window->show();
    }
}

void MainWindow::evaluationGraph(QListWidgetItem* item)
{
    this->evaluationTest();
}

void MainWindow::evaluationTest()
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
        qDebug() << "Draw graphs for evaluation " << path1;
        GraphsEvalWindow * window = new GraphsEvalWindow(path1, new DrawerEvalPitch());

        connect(window, SIGNAL(autoRec()), this, SLOT(autoRecording()));
        connect(window, SIGNAL(rec()), this, SLOT(manualRecording()));

        connect(window, SIGNAL(recFinish()), this, SLOT(updateFileList()));

        window->show();
        window->drawFile(path2);
    }
}

void MainWindow::evaluationF0()
{
    evaluation(new DrawerEvalPitch());
}

void MainWindow::evaluationI()
{
    evaluation(new DrawerEvalEnergy());
}

void MainWindow::evaluation(Drawer * drawer)
{
    QString path = QApplication::applicationDirPath() + DATA_PATH;
    QList<QListWidgetItem*> items = ui->filesList->selectedItems();
    if(items.size() > 0)
    {
        QString file = path + items.at(0)->text();
        qDebug() << "Draw graphs for evaluation " << file;
        GraphsEvalWindow * window = new GraphsEvalWindow(file, drawer);

        connect(window, SIGNAL(autoRec()), this, SLOT(autoRecording()));
        connect(window, SIGNAL(rec()), this, SLOT(manualRecording()));

        connect(window, SIGNAL(recFinish()), this, SLOT(updateFileList()));

        window->show();
    }
}

void MainWindow::plotting()
{
    QString path = QApplication::applicationDirPath() + DATA_PATH;
    QList<QListWidgetItem*> items = ui->filesList->selectedItems();
    if(items.size() > 0)
    {
        QString file = path + items.at(0)->text();
        qDebug() << "Draw graphs for " << path;
        this->showGraph(file)->show();
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
            QFile(path + orig_name).rename(QDir(path).absoluteFilePath(new_name + WAVE_TYPE));
        }
    }
    this->updateFileList();
}

void MainWindow::settingsShow()
{
    if(this->settingsDialog)
        this->settingsDialog->show();
}

void MainWindow::playRecord()
{
    QString path = QApplication::applicationDirPath() + DATA_PATH;
    QList<QListWidgetItem*> items = ui->filesList->selectedItems();
    for(int i=0; i<items.size(); i++)
    {
        qDebug() << "play file " << items.at(i)->text();
        QString file = path + items.at(i)->text();
        SoundPlayer * player = new SoundPlayer(file);
        player->start();
    }
}
