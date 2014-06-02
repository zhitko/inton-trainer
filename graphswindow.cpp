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
#include "autosoundrecorder.h"

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
    drawer(NULL)
{
    this->initUI();
    this->ui->fileNameLabel->setText(tr("Training"));
}

GraphsWindow::GraphsWindow(QString path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphsWindow),
    lastImageFile(""),
    path(path),
    k_graph(GRAPH_K_INIT),
    drawer(NULL)
{
    this->initUI();
    this->fileName = path.left(path.length()-4);

    this->ui->fileNameLabel->setText(QUrl(path).fileName());

    this->ui->tabWidget->removeTab(
        this->ui->tabWidget->indexOf(this->ui->tab_training)
    );

    this->drawFile(path);
}

void GraphsWindow::initUI()
{
    ui->setupUi(this);

    connect(this->ui->applyBtn, SIGNAL(clicked()), this, SLOT(rangeChanged()));
    connect(this->ui->pitchAutoBtn, SIGNAL(clicked()), this, SLOT(pitchAuto()));
    connect(this->ui->specAutoBtn, SIGNAL(clicked()), this, SLOT(specAuto()));
    connect(this->ui->to3DBtn, SIGNAL(clicked()), this, SLOT(stereo()));

    connect(this->ui->recordBtn, SIGNAL(clicked()), this, SLOT(_rec()));
    connect(this->ui->autoRecordBtn, SIGNAL(clicked()), this, SLOT(_autoRec()));

    connect(this->ui->lessBtn, SIGNAL(clicked()), this, SLOT(decrease()));
    connect(this->ui->moreBtn, SIGNAL(clicked()), this, SLOT(increase()));
    connect(this->ui->fitBtn, SIGNAL(clicked()), this, SLOT(fit()));
    connect(this->ui->saveImage, SIGNAL(clicked()), this, SLOT(saveImage()));
    connect(this->ui->openImage, SIGNAL(clicked()), this, SLOT(openImage()));

    connect(this->ui->playBtn, SIGNAL(clicked()), this, SLOT(playRecord()));

    connect(this->ui->setRecordBtn, SIGNAL(clicked()), this, SLOT(startRecord()));

    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
    this->ui->pitchMinSpin->setValue(sptk_settings->pitch->min_freq);
    this->ui->pitchMaxSpin->setValue(sptk_settings->pitch->max_freq);

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
    this->drawer = createNewDrawer(path);
    this->w_graph = this->drawer->getDataLenght();
    this->QMGL->setDraw(this->drawer);
    this->setFitByK();
}

Drawer * GraphsWindow::createNewDrawer(QString path)
{
    if(this->drawer) delete drawer;
    return new Drawer(path);
}

void GraphsWindow::startRecord()
{
    qDebug() << "GraphsWindow::setRecord";
    this->ui->setRecordBtn->setEnabled(false);
    oal_device * currentDevice = SettingsDialog::getInstance()->getInputDevice();
    AutoSoundRecorder * autoRecorder = new AutoSoundRecorder(currentDevice, sizeof(short int), this);
    connect(autoRecorder, SIGNAL(resultReady(SoundRecorder *)), this, SLOT(stopRecord(SoundRecorder *)));
    autoRecorder->startRecording();
}

void GraphsWindow::stopRecord(SoundRecorder * recorder)
{
    qDebug() << "GraphsWindow::stopRecord";
    this->ui->setRecordBtn->setEnabled(true);
    char *data;
    int size = recorder->getData((void**) &data);
    recorder->deleteLater();
    QDateTime dateTime = QDateTime::currentDateTime();
    QString path = USER_DATA_PATH + dateTime.toString("dd.MM.yyyy hh:mm:ss.zzz");

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

GraphsWindow::~GraphsWindow()
{
    delete ui;
}

void GraphsWindow::rangeChanged()
{
    this->drawer->pitchMin =  ((double) this->ui->pitchMinSpin->value());
    this->drawer->pitchMax =  ((double) this->ui->pitchMaxSpin->value());
    this->drawer->specMin =  ((double) this->ui->specMinSpin->value()) / this->ui->specMinSpin->maximum();
    this->drawer->specMax =  ((double) this->ui->specMaxSpin->value()) / this->ui->specMaxSpin->maximum();
    this->QMGL->update();
}


void GraphsWindow::pitchAuto()
{
    this->drawer->pitchAuto();
    this->ui->pitchMinSpin->setValue(this->drawer->pitchMin);
    this->ui->pitchMaxSpin->setValue(this->drawer->pitchMax);
    this->QMGL->update();
}

void GraphsWindow::specAuto()
{
    this->drawer->specAuto();
    this->ui->specMinSpin->setValue(this->drawer->specMin * this->ui->specMinSpin->maximum());
    this->ui->specMaxSpin->setValue(this->drawer->specMax * this->ui->specMaxSpin->maximum());
    this->QMGL->update();
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
    this->k_graph = GRAPH_K_INIT;
    this->setFitByK();
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
