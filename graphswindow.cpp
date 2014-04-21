#include "graphswindow.h"
#include "ui_graphswindow.h"
#include <QScrollArea>
#include <QFileDialog>
#include <QDesktopServices>

#include "settingsdialog.h"
#include "drawer.h"

#include <QDebug>

extern "C" {
    #include "./OpenAL/wavFile.h"

    #include "float.h"

    #include "./SPTK/SPTK.h"
    #include "./SPTK/pitch/pitch.h"
    #include "./SPTK/x2x/x2x.h"
    #include "./SPTK/frame/frame.h"
    #include "./SPTK/window/window.h"
    #include "./SPTK/lpc/lpc.h"
    #include "./SPTK/spec/spec.h"
}

GraphsWindow::GraphsWindow(QString path, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphsWindow),
    lastImageFile("")
{
    ui->setupUi(this);
    this->fileName = path.left(path.length()-4);
    this->showGraph(path);
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
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
    this->ui->pitchMinSpin->setValue(sptk_settings->pitch->min_freq);
    this->ui->pitchMaxSpin->setValue(sptk_settings->pitch->max_freq);

    this->ui->fileNameLabel->setText(QUrl(path).fileName());
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
//    this->QMGL->waveMin =
//    this->QMGL->waveMax =
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

void GraphsWindow::showGraph(QString path)
{
    QMGL = new QMathGL(this);
    QMGL->autoResize = false;
    QMGL->enableWheel = false;
    QMGL->enableMouse = false;
    drawer = new Drawer(path);
    QMGL->setDraw(drawer);
    scrollArea = new QScrollArea(this);\
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->ui->horizontalLayout_2->addWidget(scrollArea, 0);
    scrollArea->setBackgroundRole(QPalette::Light);
    scrollArea->setWidget(QMGL);
    QMGL->setSize(drawer->getDataLenght()/GRAPH_WIGHT_K, GRAPH_HEIGHT);
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
    QSize size = this->scrollArea->maximumViewportSize();
    int new_size = this->QMGL->width() + by;
    this->QMGL->setSize(new_size, size.height());
    emit this->moreSig(by);
}

void GraphsWindow::decrease()
{
    int by = this->ui->scaleStepSpin->value();
    this->decrease(by);
}

void GraphsWindow::decrease(int by)
{
    QSize size = this->scrollArea->maximumViewportSize();
    int new_size = this->QMGL->width() - by;
    if(new_size <= 0) new_size = 1;
    this->QMGL->setSize(new_size, size.height());
    emit this->lessSig(by);
}

void GraphsWindow::fit()
{
    QSize size = this->scrollArea->maximumViewportSize();
    this->QMGL->setSize(drawer->getDataLenght()/GRAPH_WIGHT_K, size.height());
    emit this->fitSig();
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
