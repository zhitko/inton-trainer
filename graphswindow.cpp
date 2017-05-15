#include "graphswindow.h"
#include "ui_graphswindow.h"
#include <QScrollArea>
#include <QFileDialog>
#include <QDesktopServices>
#include <QDateTime>

#include "mainwindow.h"
#include "settingsdialog.h"
#include "drawer.h"
#include "soundplayer.h"
#include "Recorder/autosoundrecorder.h"

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

    this->ui->tabWidget->removeTab(
        this->ui->tabWidget->indexOf(this->ui->tab_training)
    );

    this->drawFile(path);
}

GraphsWindow::~GraphsWindow()
{
    delete ui;
    this->drawer;
}

void GraphsWindow::initUI()
{
    ui->setupUi(this);
    connect(this->ui->to3DBtn, SIGNAL(clicked()), this, SLOT(stereo()));

    connect(this->ui->recordBtn, SIGNAL(clicked()), this, SLOT(_rec()));

    connect(this->ui->lessBtn, SIGNAL(clicked()), this, SLOT(decrease()));
    connect(this->ui->moreBtn, SIGNAL(clicked()), this, SLOT(increase()));
    connect(this->ui->fitBtn, SIGNAL(clicked()), this, SLOT(fit()));
    connect(this->ui->saveImage, SIGNAL(clicked()), this, SLOT(saveImage()));
    connect(this->ui->openImage, SIGNAL(clicked()), this, SLOT(openImage()));

    connect(this->ui->playBtn, SIGNAL(clicked()), this, SLOT(playRecord()));

    connect(this->ui->autoRecordBtn, SIGNAL(clicked()), this, SLOT(_autoRec()));
    connect(this->ui->setRecordBtn, SIGNAL(clicked()), this, SLOT(startRecord()));
    connect(this->ui->stopRecordBtn, SIGNAL(clicked()), this, SLOT(stopRecord()));
    connect(this->ui->startAutoRecordBtn, SIGNAL(clicked()), this, SLOT(startAutoRecord()));

    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
    this->ui->pitchMinSpin->setValue(sptk_settings->pitch->MIN_FREQ);
    this->ui->pitchMaxSpin->setValue(sptk_settings->pitch->MAX_FREQ);

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

void GraphsWindow::drawFile(QString path)
{
    this->path = path;
    this->drawer = this->createNewDrawer(path);
    this->w_graph = this->drawer->getDataLenght();
    this->QMGL->update();
}

Drawer * GraphsWindow::createNewDrawer(QString path)
{
    Drawer * drawer = new Drawer();
    drawer->Proc(path);
    this->QMGL->setDraw(drawer);
    return drawer;
}

void GraphsWindow::startAutoRecord()
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    qDebug() << "GraphsWindow::startAutoRecord";
    this->ui->setRecordBtn->setEnabled(false);
    this->ui->startAutoRecordBtn->setEnabled(false);
    this->ui->stopRecordBtn->setEnabled(true);
    oal_device * currentDevice = SettingsDialog::getInstance()->getInputDevice();
    if (this->recorder) {
        this->recorder->deleteLater();
    }
    this->recorder = new AutoSoundRecorder(currentDevice, sizeof(short int));
    connect(this->recorder, SIGNAL(resultReady(SoundRecorder *)), this, SLOT(stopRecord(SoundRecorder *)));
    this->recorder->startRecording();
}

void GraphsWindow::startRecord()
{
    qDebug() << "GraphsWindow::setRecord";
    this->ui->setRecordBtn->setEnabled(false);
    this->ui->startAutoRecordBtn->setEnabled(false);
    this->ui->stopRecordBtn->setEnabled(true);
    oal_device * currentDevice = SettingsDialog::getInstance()->getInputDevice();
    this->recorder = new AutoSoundRecorder(currentDevice, sizeof(short int), -1, 2);
    connect(this->recorder, SIGNAL(resultReady(SoundRecorder *)), this, SLOT(stopRecord(SoundRecorder *)));
    this->recorder->startRecording();
}

void GraphsWindow::stopRecord()
{
    this->recorder->stopRecording();
}

void GraphsWindow::stopRecord(SoundRecorder * recorder)
{
    qDebug() << "GraphsWindow::stopRecord";
    this->ui->setRecordBtn->setEnabled(true);
    this->ui->startAutoRecordBtn->setEnabled(true);
    this->ui->stopRecordBtn->setEnabled(false);
    char *data;
    int size = recorder->getData((void**) &data);
    recorder->deleteLater();
    QDateTime dateTime = QDateTime::currentDateTime();
    QString path = USER_DATA_PATH + dateTime.toString("dd.MM.yyyy hh.mm.ss.zzz");

    path = QApplication::applicationDirPath() + DATA_PATH + path + WAVE_TYPE;
    WaveFile *waveFile = makeWaveFileFromData((char *)data, size, 1, 8000, 16);
    saveWaveFile(waveFile, path.toLocal8Bit().data());
    waveCloseFile(waveFile);

    this->drawFile(path);
    emit this->changeSig(this->k_graph);
    emit this->recFinish();
}

void GraphsWindow::_autoRec()
{
    emit autoRec();
}

void GraphsWindow::_rec()
{
    emit rec();
}

void GraphsWindow::hideZoomControls()
{
    this->ui->lessBtn->hide();
    this->ui->moreBtn->hide();
    this->ui->fitBtn->hide();
    this->ui->scaleStepSpin->hide();
    this->ui->sizeLabel->hide();
    this->ui->recordLabel->hide();
    this->ui->autoRecordBtn->hide();
    this->ui->recordBtn->hide();
}

void GraphsWindow::increase()
{
    int by = this->ui->scaleStepSpin->value();
    this->increase(by);
}

void GraphsWindow::increase(int by)
{
    this->k_graph += by;
    if(this->k_graph > GRAPH_K_MAX) this->k_graph = GRAPH_K_MAX;
    this->setFitByK();
    emit this->changeSig(this->k_graph);
}

void GraphsWindow::decrease()
{
    int by = this->ui->scaleStepSpin->value();
    this->decrease(by);
}

void GraphsWindow::decrease(int by)
{
    this->k_graph -= by;
    if(this->k_graph < 1) this->k_graph = 1;
    this->setFitByK();
    emit this->changeSig(this->k_graph);
}

void GraphsWindow::fit()
{
    this->fullFit();
    emit this->changeSig(this->k_graph);
}

void GraphsWindow::setK(int k)
{
    this->k_graph = k;
    this->setFitByK();
}

void GraphsWindow::setFitByK()
{
    QSize size = this->scrollArea->maximumViewportSize();
    if(!this->isVisible()) size.setHeight(GRAPH_HEIGHT);
    int width = (this->w_graph / GRAPH_K_MAX)* this->k_graph;
    this->QMGL->setSize(width, size.height());
}

void GraphsWindow::fullFit()
{
    QSize size = this->scrollArea->maximumViewportSize();
    if(!this->isVisible()) size.setHeight(GRAPH_HEIGHT);
    this->QMGL->setSize(size.width(), size.height());
}

void GraphsWindow::saveImage()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               this->fileName,
                               tr("Images (*.png)"));
    QString fname = fileName.left(fileName.lastIndexOf("\."));
    this->QMGL->exportPNGs(fname);
    this->lastImageFile = fname + ".png";
}

void GraphsWindow::openImage()
{
    if(!this->lastImageFile.isEmpty())
        QDesktopServices::openUrl(QUrl(this->lastImageFile));
}

void GraphsWindow::stereo()
{
    this->drawer->stereo = !this->drawer->stereo;
    this->QMGL->update();
}

void GraphsWindow::playRecord()
{
    SoundPlayer * player = new SoundPlayer(path);
    player->start();
}
