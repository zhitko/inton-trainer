#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QDebug>
#include <QSettings>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    inputDevices(NULL), outputDevices(NULL),
    currentInputDevice(NULL), currentOutputDevice(NULL)
{
    ui->setupUi(this);
    this->loadSettings();
    this->initAudio();
    this->initUI();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
    if(currentOutputDevice) freeAudioOutputDevice(currentOutputDevice);
}

SettingsDialog * SettingsDialog::getInstance(QWidget *parent)
{
    static SettingsDialog * instance = new SettingsDialog(parent);
    return instance;
}

void SettingsDialog::initAudio()
{
    inputDevices = getInputDevices();
    outputDevices = getOutputDevices();

    if(inputDevices) currentInputDevice = inputDevices->device;
    if(outputDevices) currentOutputDevice = outputDevices->device;
    qDebug() << "currentInputDevice " << QString::fromLocal8Bit(this->currentInputDevice->name);
    qDebug() << "currentOutputDevice " << QString::fromLocal8Bit(this->currentOutputDevice->name);
}

void SettingsDialog::inputDeviceChanged(int index)
{
    oal_devices_list *list = this->inputDevices;
    for(int i=0;i<index;i++) list = list->next;
    this->currentInputDevice = list->device;
    qDebug() << "inputDeviceChanged " << QString::fromLocal8Bit(this->currentInputDevice->name);
}

void SettingsDialog::outputDeviceChanged(int index)
{
    oal_devices_list *list = this->outputDevices;
    for(int i=0;i<index;i++) list = list->next;
    this->currentOutputDevice = list->device;
    qDebug() << "outputDeviceChanged " << QString::fromLocal8Bit(this->currentOutputDevice->name);
}

void SettingsDialog::initUI()
{
    oal_devices_list *list = this->inputDevices;
    while(list)
    {
        QString name = QString::fromLocal8Bit(list->device->name);
        this->ui->audioInputDeviceBox->addItem(name);
        list = list->next;
    }
    connect(this->ui->audioInputDeviceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(inputDeviceChanged(int)));

    list = this->outputDevices;
    while(list)
    {
        QString name = QString::fromLocal8Bit(list->device->name);
        this->ui->audioOutputDeviceBox->addItem(name);
        list = list->next;
    }
    connect(this->ui->audioOutputDeviceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(outputDeviceChanged(int)));

    connect(this->ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveSettings()));
}

oal_device * SettingsDialog::getInputDevice()
{
    return this->currentInputDevice;
}

oal_device * SettingsDialog::getOutputDevice()
{
    return this->currentOutputDevice;
}

SPTK_SETTINGS * SettingsDialog::getSPTKsettings()
{
    SPTK_SETTINGS * sptk_settings = initSptkSettings();

    SettingsDialog * instance = getInstance();

//    sptk_settings->pitch->SAMPLE_FREQ = instance->ui->pitchSampleFreqSpin->value();
    sptk_settings->pitch->MAX_FREQ = instance->ui->pitchMaxFreqSpin->value();
    sptk_settings->pitch->MIN_FREQ = instance->ui->pitchMinFreqSpin->value();
    sptk_settings->pitch->FRAME_SHIFT = instance->ui->pitchFrameShoftSpin->value();
    sptk_settings->pitch->ATYPE = instance->ui->pitchATypeSpin->value();
    sptk_settings->pitch->OTYPE = instance->ui->pitchOTypeSpin->value();
    sptk_settings->pitch->THRESH_RAPT = instance->ui->pitchThreshRaptSpin->value();
    sptk_settings->pitch->THRESH_SWIPE = instance->ui->pitchThreshSwipeSpin->value();

    sptk_settings->energyFrame->leng = instance->ui->energyMidFrame->value();
    sptk_settings->energyFrame->threshold_start = instance->ui->energyMidThresholdStart->value();
    sptk_settings->energyFrame->threshold_end = instance->ui->energyMidThresholdEnd->value();

    sptk_settings->plotF0->midFrame = instance->ui->plotMidFrameF0Spin->value();
    sptk_settings->plotF0->interpolation_edges = instance->ui->plotInterpolationEdgesF0Spin->value();
    sptk_settings->plotF0->interpolation_type = instance->ui->plotInterpolationTypeF0Box->currentIndex();

    sptk_settings->plotEnergy->midFrame = instance->ui->plotMidFrameEnergySpin->value();
    sptk_settings->plotEnergy->interpolation_edges = instance->ui->plotInterpolationEdgesEnergySpin->value();
    sptk_settings->plotEnergy->interpolation_type = instance->ui->plotInterpolationTypeEnergyBox->currentIndex();

    sptk_settings->frame->leng = instance->ui->frameSizeBox->currentText().toInt();
    sptk_settings->frame->shift = instance->ui->frameShiftSpin->value();
    sptk_settings->window->leng = instance->ui->frameSizeBox->currentText().toInt();
    sptk_settings->window->window_type = static_cast<Window>(instance->ui->windoTypeBox->currentIndex());
    sptk_settings->lpc->leng = instance->ui->frameSizeBox->currentText().toInt();
    sptk_settings->lpc->order = instance->ui->lpcOrderSpin->value();
    sptk_settings->spec->leng = instance->ui->frameSizeBox->currentText().toInt();
    sptk_settings->spec->order = instance->ui->lpcOrderSpin->value();

    return sptk_settings;
}

MathGLSettings * SettingsDialog::getMathGLSettings()
{
    MathGLSettings * settings = new MathGLSettings();
    SettingsDialog * instance = getInstance();
    settings->quality = instance->ui->mathGLQualitySpin->value();
    settings->autoOpen = instance->ui->isAutoOpen->isChecked();
    return settings;
}

void SettingsDialog::loadSettings()
{
    qDebug() << "loadSettings from " << SETTINGS_FILE;
    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
    settings.setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());

    if(settings.contains("pitch/max_freq"))
        this->ui->pitchMaxFreqSpin->setValue(settings.value("pitch/max_freq").toInt());
    if(settings.contains("pitch/min_freq"))
        this->ui->pitchMinFreqSpin->setValue(settings.value("pitch/min_freq").toInt());

//    if(settings.contains("pitch/SAMPLE_FREQ"))
//        this->ui->pitchSampleFreqSpin->setValue(settings.value("pitch/SAMPLE_FREQ").toDouble());
    if(settings.contains("pitch/FRAME_SHIFT"))
        this->ui->pitchFrameShoftSpin->setValue(settings.value("pitch/FRAME_SHIFT").toInt());
    if(settings.contains("pitch/ATYPE"))
        this->ui->pitchATypeSpin->setValue(settings.value("pitch/ATYPE").toInt());
    if(settings.contains("pitch/OTYPE"))
        this->ui->pitchOTypeSpin->setValue(settings.value("pitch/OTYPE").toInt());
    if(settings.contains("pitch/THRESH_RAPT"))
        this->ui->pitchThreshRaptSpin->setValue(settings.value("pitch/THRESH_RAPT").toDouble());
    if(settings.contains("pitch/THRESH_SWIPE"))
        this->ui->pitchThreshSwipeSpin->setValue(settings.value("pitch/THRESH_SWIPE").toDouble());

    if(settings.contains("plot_f0/mid_frame"))
        this->ui->plotMidFrameF0Spin->setValue(settings.value("plot_f0/mid_frame").toInt());
    if(settings.contains("plot_f0/interpolation_type"))
        this->ui->plotInterpolationTypeF0Box->setCurrentIndex(settings.value("plot_f0/interpolation_type").toInt());
    if(settings.contains("plot_f0/interpolation_edges"))
        this->ui->plotInterpolationEdgesF0Spin->setValue(settings.value("plot_f0/interpolation_edges").toInt());

    if(settings.contains("plot_energy/mid_frame"))
        this->ui->plotMidFrameEnergySpin->setValue(settings.value("plot_energy/mid_frame").toInt());
    if(settings.contains("plot_energy/interpolation_type"))
        this->ui->plotInterpolationTypeEnergyBox->setCurrentIndex(settings.value("plot_energy/interpolation_type").toInt());
    if(settings.contains("plot_energy/interpolation_edges"))
        this->ui->plotInterpolationEdgesEnergySpin->setValue(settings.value("plot_energy/interpolation_edges").toInt());

    if(settings.contains("energyFrame/leng"))
        this->ui->energyMidFrame->setValue(settings.value("energyFrame/leng").toInt());
    if(settings.contains("energyFrame/threshold_start"))
        this->ui->energyMidThresholdStart->setValue(settings.value("energyFrame/threshold_start").toDouble());
    if(settings.contains("energyFrame/threshold_end"))
        this->ui->energyMidThresholdEnd->setValue(settings.value("energyFrame/threshold_end").toDouble());

    if(settings.contains("frame/leng"))
        this->ui->frameSizeBox->setCurrentText(QString::number(settings.value("frame/leng").toInt()));
    if(settings.contains("frame/shift"))
        this->ui->frameShiftSpin->setValue(settings.value("frame/shift").toInt());
//    if(settings.contains("window/leng"))
//        this->ui->frameSizeBox->setCurrentText(QString::number(settings.value("window/leng").toInt()));
    if(settings.contains("window/window_type"))
        this->ui->windoTypeBox->setCurrentText(settings.value("window/window_type").toString());
//    if(settings.contains("lpc/leng"))
//        this->ui->frameSizeBox->setCurrentText(QString::number(settings.value("lpc/leng").toInt()));
    if(settings.contains("lpc/order"))
        this->ui->lpcOrderSpin->setValue(settings.value("lpc/order").toInt());
//    if(settings.contains("spec/leng"))
//        this->ui->frameSizeBox->setCurrentText(QString::number(settings.value("spec/leng").toInt()));
//    if(settings.contains("spec/order"))
//        this->ui->lpcOrderSpin->setValue(settings.value("spec/order").toInt());
    if(settings.contains("mathGL/quality"))
        this->ui->mathGLQualitySpin->setValue(settings.value("mathGL/quality").toInt());
    if(settings.contains("mathGL/autoOpen"))
        this->ui->isAutoOpen->setChecked(settings.value("mathGL/autoOpen").toBool());

}

void SettingsDialog::saveSettings()
{
    qDebug() << "saveSettings to " << SETTINGS_FILE;
    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
    settings.setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());

    settings.setValue("pitch/max_freq", this->ui->pitchMaxFreqSpin->value());
    settings.setValue("pitch/min_freq", this->ui->pitchMinFreqSpin->value());

//    settings.setValue("pitch/SAMPLE_FREQ", this->ui->pitchSampleFreqSpin->value());
    settings.setValue("pitch/FRAME_SHIFT", this->ui->pitchFrameShoftSpin->value());
    settings.setValue("pitch/ATYPE", this->ui->pitchATypeSpin->value());
    settings.setValue("pitch/OTYPE", this->ui->pitchOTypeSpin->value());
    settings.setValue("pitch/THRESH_RAPT", this->ui->pitchThreshRaptSpin->value());
    settings.setValue("pitch/THRESH_SWIPE", this->ui->pitchThreshSwipeSpin->value());

    settings.setValue("plot_f0/mid_frame", this->ui->plotMidFrameF0Spin->value());
    settings.setValue("plot_f0/interpolation_edges", this->ui->plotInterpolationEdgesF0Spin->value());
    settings.setValue("plot_f0/interpolation_type", this->ui->plotInterpolationTypeF0Box->currentIndex());

    settings.setValue("plot_energy/mid_frame", this->ui->plotMidFrameEnergySpin->value());
    settings.setValue("plot_energy/interpolation_edges", this->ui->plotInterpolationEdgesEnergySpin->value());
    settings.setValue("plot_energy/interpolation_type", this->ui->plotInterpolationTypeEnergyBox->currentIndex());

    settings.setValue("energyFrame/leng", this->ui->energyMidFrame->value());
    settings.setValue("energyFrame/threshold_start", this->ui->energyMidThresholdStart->value());
    settings.setValue("energyFrame/threshold_end", this->ui->energyMidThresholdEnd->value());

    settings.setValue("frame/leng", this->ui->frameSizeBox->currentText().toInt());
    settings.setValue("frame/shift", this->ui->frameShiftSpin->value());
    settings.setValue("window/leng", this->ui->frameSizeBox->currentText().toInt());
    settings.setValue("window/window_type", this->ui->windoTypeBox->currentText());
    settings.setValue("lpc/leng", this->ui->frameSizeBox->currentText().toInt());
    settings.setValue("lpc/order", this->ui->lpcOrderSpin->value());
    settings.setValue("spec/leng", this->ui->frameSizeBox->currentText().toInt());
    settings.setValue("spec/order", this->ui->lpcOrderSpin->value());

    settings.setValue("mathGL/quality", this->ui->mathGLQualitySpin->value());
    settings.setValue("mathGL/autoOpen", this->ui->isAutoOpen->isChecked());
}
