#include "graphswindow.h"
#include "ui_graphswindow.h"
#include <QScrollArea>
#include <QFileDialog>
#include <QDesktopServices>
#include <QDateTime>
#include <QSizePolicy>

#include "mainwindow.h"
#include "settingsdialog.h"
#include "drawer.h"
#include "soundplayer.h"
#include "Recorder/autosoundrecorder.h"
#include "Recorder/timesoundrecorder.h"

#include <QDebug>

extern "C" {
    #include "./OpenAL/wavFile.h"
    #include "./OpenAL/openal_wrapper.h"

    #include "float.h"

    #include "./SPTK/SPTK.h"
    #include "./SPTK/pitch/pitch.h"
    #include "./SPTK/x2x/x2x.h"
    #include "./SPTK/frame/frame.h"
    #include "./SPTK/window/window.h"
    #include "./SPTK/lpc/lpc.h"
    #include "./SPTK/spec/spec.h"
}

GraphsWindow::GraphsWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphsWindow),
    lastImageFile(""),
    path(""),
    fileName(""),
    k_graph(GRAPH_K_INIT),
    drawer(NULL),
    recorder(NULL)
{
    this->initUI();

    setAttribute( Qt::WA_DeleteOnClose );
}

GraphsWindow::GraphsWindow(QString path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphsWindow),
    lastImageFile(""),
    path(path),
    k_graph(GRAPH_K_INIT),
    drawer(NULL),
    recorder(NULL)
{
    this->initUI();

    setAttribute( Qt::WA_DeleteOnClose );

    this->fileName = path.left(path.length()-4);
    this->drawFile(path);

//    this->resizeTimer = new QTimer(this);
//    connect(resizeTimer, SIGNAL(timeout()),this, SLOT(fullFit()));
}

GraphsWindow::~GraphsWindow()
{
    delete ui;
    this->drawer;
}

void GraphsWindow::initUI()
{
    ui->setupUi(this);
    this->setWindowTitle(FULL_APP_VERIOSN);

    connect(this->ui->playBtn, SIGNAL(clicked()), this, SLOT(playRecord()));
    connect(this->ui->setRecordBtn, SIGNAL(clicked()), this, SLOT(startRecord()));

    connect(this->ui->openFileBtn, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(this->ui->openRefFileBtn, SIGNAL(clicked()), this, SLOT(openRefFile()));

    QMGL = new QMathGL(this);
    QMGL->autoResize = false;
    QMGL->enableWheel = false;
    QMGL->enableMouse = false;
    scrollArea = new QScrollArea(this);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->ui->horizontalLayout_2->addWidget(scrollArea, 0);
    scrollArea->setBackgroundRole(QPalette::Light);
    scrollArea->setWidget(QMGL);
}

void GraphsWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if(this->resizeTimer->isActive())
    {
        this->resizeTimer->stop();
    }
    this->resizeTimer->start(1000);
}

void GraphsWindow::fullFit()
{
    QSize size = this->scrollArea->maximumViewportSize();
    if(!this->isVisible()) size.setHeight(GRAPH_HEIGHT);
    this->QMGL->setSize(size.width()-5, size.height()-5);
}

void GraphsWindow::drawFile(QString path, bool ref)
{
    qDebug() << "GraphsWindow::drawFile" << LOG_DATA;
    this->path = path;
    this->drawer = this->createNewDrawer(path, ref);
    this->w_graph = this->drawer->getDataLenght();
    this->QMGL->update();

    this->resizeTimer = new QTimer(this);
    connect(resizeTimer, SIGNAL(timeout()),this, SLOT(fullFit()));
}

Drawer * GraphsWindow::createNewDrawer(QString path, bool ref)
{
    Drawer * drawer = new Drawer();
    drawer->Proc(path);
    this->QMGL->setDraw(drawer);
    return drawer;
}

void GraphsWindow::startAutoRecord()
{
    oal_device * currentDevice = SettingsDialog::getInstance()->getInputDevice();
    if (!currentDevice) return;

    qDebug() << "GraphsWindow::startAutoRecord" << LOG_DATA;
    this->ui->setRecordBtn->setEnabled(false);
    this->ui->openFileBtn->setEnabled(false);
    this->ui->openRefFileBtn->setEnabled(false);

    if (this->recorder) {
        this->recorder->deleteLater();
    }
    this->recorder = new AutoSoundRecorder(currentDevice, sizeof(short int));
    connect(this->recorder, SIGNAL(resultReady(SoundRecorder *)), this, SLOT(stopRecord(SoundRecorder *)));
    this->recorder->startRecording();
}

void GraphsWindow::openRefFile()
{
    this->ui->openFileBtn->hide();
    this->ui->showAutomatically->hide();
    this->ui->showManually->setEnabled(false);
    this->ui->showSource->hide();
    this->ui->setRecordBtn->hide();
    this->ui->showUMP->setEnabled(false);
    this->openFile(true);
}

void GraphsWindow::openFile(bool ref)
{
    QString initPath;

    if (ref)
    {
        initPath = this->path;
    } else {
        initPath = QApplication::applicationDirPath() + DATA_PATH_TEST;
    }

    QString path = QFileDialog::getOpenFileName(this, tr("Open Record"), initPath, tr("WAV Files (*.wav)"));

    qDebug() << "GraphsWindow::openFile " << path << LOG_DATA;

    if (!path.isEmpty())
    {
        this->drawFile(path, ref);
        qDebug() << "GraphsWindow::openFile length=" << w_graph << LOG_DATA;
    }
}

void GraphsWindow::startRecord()
{
    oal_device * currentDevice = SettingsDialog::getInstance()->getInputDevice();
    if (!currentDevice) return;

    if (this->recorder && this->recorder->isRecording())
    {
        this->recorder->stopRecording();
    } else {
        SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
        qDebug() << "GraphsWindow::startRecord" << LOG_DATA;
        this->ui->setRecordBtn->setEnabled(false);
        this->ui->openFileBtn->setEnabled(false);
        this->ui->openRefFileBtn->setEnabled(false);
        if (sptk_settings->dp->recordingType == 0) // Recording N sec
        {
            this->recorder = new TimeSoundRecorder(currentDevice, sizeof(short int), sptk_settings->dp->recordingSeconds);
        } else if (sptk_settings->dp->recordingType == 1) // Manual recording
        {
            this->ui->setRecordBtn->setEnabled(true);
            this->ui->openFileBtn->setEnabled(true);
            this->ui->openRefFileBtn->setEnabled(true);
            this->recorder = new SoundRecorder(currentDevice, sizeof(short int));
        } else if (sptk_settings->dp->recordingType == 2) // Recording by F0
        {
            this->recorder = new AutoSoundRecorder(currentDevice, sizeof(short int), -1, 2);
        } else if (sptk_settings->dp->recordingType == 3) // Recording N + template sec
        {
            int seconds = sptk_settings->dp->recordingSeconds + this->drawer->getDataSeconds();
            qDebug() << "GraphsWindow::startRecord seconds " << seconds << LOG_DATA;
            this->recorder = new TimeSoundRecorder(currentDevice, sizeof(short int), seconds);
        } else {
            this->recorder = new AutoSoundRecorder(currentDevice, sizeof(short int), -1, 2);
        }

        connect(this->recorder, SIGNAL(resultReady(SoundRecorder *)), this, SLOT(stopRecord(SoundRecorder *)));
        this->recorder->startRecording();
    }
}

void GraphsWindow::stopRecord()
{
    this->recorder->stopRecording();
}

void GraphsWindow::stopRecord(SoundRecorder * recorder)
{
    qDebug() << "GraphsWindow::stopRecord" << LOG_DATA;

    this->ui->setRecordBtn->setEnabled(true);
    this->ui->openFileBtn->setEnabled(true);
    this->ui->openRefFileBtn->setEnabled(true);
    qDebug() << "GraphsWindow::stopRecord UI changes" << LOG_DATA;

    char *data = NULL;
    int size = recorder->getData((void**) &data);
    qDebug() << "GraphsWindow::stopRecord get data" << LOG_DATA;
    QDateTime dateTime = QDateTime::currentDateTime();
    QString path = USER_DATA_PATH + dateTime.toString("dd.MM.yyyy hh.mm.ss.zzz");

    MainWindow::cleanRecordFiles();
    qDebug() << "GraphsWindow::stopRecord files cleaned" << LOG_DATA;

    path = QApplication::applicationDirPath() + DATA_PATH + path + WAVE_TYPE;
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
    qDebug() << "GraphsWindow::stopRecord makeWaveFileFromData" << LOG_DATA;
    saveWaveFile(waveFile, path.toLocal8Bit().data());
    qDebug() << "GraphsWindow::stopRecord saveWaveFile" << LOG_DATA;
//    waveCloseFile(waveFile);
    qDebug() << "GraphsWindow::stopRecord waveCloseFile" << LOG_DATA;

    this->drawFile(path);
    qDebug() << "GraphsWindow::stopRecord drawFile" << LOG_DATA;
    emit this->changeSig(this->k_graph);
    emit this->recFinish();

    recorder->deleteLater();
}

void GraphsWindow::_autoRec()
{
    emit autoRec();
}

void GraphsWindow::_rec()
{
    emit rec();
}

void GraphsWindow::playRecord()
{
    SoundPlayer * player = new SoundPlayer(path);
    player->start();
}
