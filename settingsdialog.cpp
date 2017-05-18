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
    qDebug() << "currentInputDevice " << this->currentInputDevice->name;
    qDebug() << "currentOutputDevice " << this->currentOutputDevice->name;
}

void SettingsDialog::inputDeviceChanged(int index)
{
    oal_devices_list *list = this->inputDevices;
    for(int i=0;i<index;i++) list = list->next;
    this->currentInputDevice = list->device;
    qDebug() << "inputDeviceChanged " << this->currentInputDevice->name;
}

void SettingsDialog::outputDeviceChanged(int index)
{
    oal_devices_list *list = this->outputDevices;
    for(int i=0;i<index;i++) list = list->next;
    this->currentOutputDevice = list->device;
    qDebug() << "outputDeviceChanged " << this->currentOutputDevice->name;
}

void SettingsDialog::initUI()
{
    oal_devices_list *list = this->inputDevices;
    while(list)
    {
        QString name = list->device->name;
        this->ui->audioInputDeviceBox->addItem(name);
        list = list->next;
    }
    connect(this->ui->audioInputDeviceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(inputDeviceChanged(int)));

    list = this->outputDevices;
    while(list)
    {
        QString name = list->device->name;
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
    instance->loadSettings();

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
//    sptk_settings->plotF0->interpolation_edges = instance->ui->plotInterpolationEdgesF0Spin->value();
    sptk_settings->plotF0->interpolation_type = instance->ui->plotInterpolationTypeF0Box->currentIndex();
    sptk_settings->plotF0->normF0MinMax = instance->ui->normF0MinMax->isChecked();

    sptk_settings->plotEnergy->midFrame = instance->ui->plotMidFrameEnergySpin->value();
    sptk_settings->plotEnergy->interpolation_edges = instance->ui->plotInterpolationEdgesEnergySpin->value();
    sptk_settings->plotEnergy->interpolation_type = instance->ui->plotInterpolationTypeEnergyBox->currentIndex();

    sptk_settings->frame->leng = instance->ui->frameSizeBox->currentText().toInt();
    sptk_settings->frame->shift = instance->ui->frameShiftSpin->value();
    sptk_settings->window->leng = instance->ui->frameSizeBox->currentText().toInt();
    sptk_settings->window->window_type = static_cast<Window>(instance->ui->windoTypeBox->currentIndex());
    sptk_settings->lpc->leng = instance->ui->frameSizeBox->currentText().toInt();
    sptk_settings->lpc->order = instance->ui->lpcOrderSpin->value();
    sptk_settings->lpc->cepstrum_order = instance->ui->lpcCepstrumOrderSpin->value();
    sptk_settings->spec->leng = instance->ui->frameSizeBox->currentText().toInt();
    sptk_settings->spec->order = instance->ui->lpcOrderSpin->value();

    sptk_settings->spec->factor = instance->ui->specFactorSpin->value();
    sptk_settings->spec->min = instance->ui->specMinSpin->value();
    sptk_settings->spec->proc = instance->ui->specProcBox->currentIndex();

    sptk_settings->move->type = instance->ui->moveTypeBox->currentIndex();

    sptk_settings->dp->globalLimit = instance->ui->dpGlobalLimit->value();
    sptk_settings->dp->localLimit = instance->ui->dpLocalLimit->value();
    sptk_settings->dp->continiusLimit = instance->ui->continiusDpLimit->value();

    sptk_settings->dp->continiusKD = instance->ui->continiusDpKD->value();
    sptk_settings->dp->continiusKH = instance->ui->continiusDpKH->value();
    sptk_settings->dp->continiusKT = instance->ui->continiusDpKT->value();
    sptk_settings->dp->continiusKV = instance->ui->continiusDpKV->value();

    sptk_settings->dp->portLen = instance->ui->portLen->value();
    sptk_settings->dp->useStripUmp = instance->ui->useStripUmp->isChecked();
    sptk_settings->dp->showA0 = instance->ui->showA0->isChecked();
    sptk_settings->dp->showPortr = instance->ui->showPortr->isChecked();
    sptk_settings->dp->showError = instance->ui->showError->isChecked();
    sptk_settings->dp->showF0 = instance->ui->showF0->isChecked();
    sptk_settings->dp->showOriginalF0 = instance->ui->showOriginalF0->isChecked();
    sptk_settings->dp->showTime = instance->ui->showTime->isChecked();
    sptk_settings->dp->errorType = instance->ui->errorTypeBox->currentIndex();
    sptk_settings->dp->useForDP = instance->ui->useForDP->currentIndex();

    sptk_settings->dp->recordingType = instance->ui->recordingType->currentIndex();
    sptk_settings->dp->recordingSeconds = instance->ui->recordingSeconds->value();

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
//    if(settings.contains("plot_f0/interpolation_edges"))
//        this->ui->plotInterpolationEdgesF0Spin->setValue(settings.value("plot_f0/interpolation_edges").toInt());
    if(settings.contains("plot_f0/normF0MinMax"))
        this->ui->normF0MinMax->setChecked(settings.value("plot_f0/normF0MinMax").toBool());

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

    if(settings.contains("spec/factor"))
        this->ui->specFactorSpin->setValue(settings.value("spec/factor").toInt());
    if(settings.contains("spec/min"))
        this->ui->specMinSpin->setValue(settings.value("spec/min").toDouble());
    if(settings.contains("spec/proc"))
        this->ui->specProcBox->setCurrentText(settings.value("spec/proc").toString());

    if(settings.contains("move/type"))
        this->ui->moveTypeBox->setCurrentText(settings.value("move/type").toString());

    if(settings.contains("dp/global"))
        this->ui->dpGlobalLimit->setValue(settings.value("dp/global").toInt());
    if(settings.contains("dp/local"))
        this->ui->dpLocalLimit->setValue(settings.value("dp/local").toDouble());
    if(settings.contains("dp/continiusLimit"))
        this->ui->continiusDpLimit->setValue(settings.value("dp/continiusLimit").toDouble());

    if(settings.contains("dp/continiusDpKV"))
        this->ui->continiusDpKV->setValue(settings.value("dp/continiusDpKV").toDouble());
    if(settings.contains("dp/continiusDpKT"))
        this->ui->continiusDpKT->setValue(settings.value("dp/continiusDpKT").toDouble());
    if(settings.contains("dp/continiusDpKH"))
        this->ui->continiusDpKH->setValue(settings.value("dp/continiusDpKH").toDouble());
    if(settings.contains("dp/continiusDpKD"))
        this->ui->continiusDpKD->setValue(settings.value("dp/continiusDpKD").toDouble());

    if(settings.contains("dp/portLen"))
        this->ui->portLen->setValue(settings.value("dp/portLen").toInt());
    if(settings.contains("dp/useStripUmp"))
        this->ui->useStripUmp->setChecked(settings.value("dp/useStripUmp").toBool());
    if(settings.contains("dp/showA0"))
        this->ui->showA0->setChecked(settings.value("dp/showA0").toBool());
    if(settings.contains("dp/showPortr"))
        this->ui->showPortr->setChecked(settings.value("dp/showPortr").toBool());
    if(settings.contains("dp/showError"))
        this->ui->showError->setChecked(settings.value("dp/showError").toBool());
    if(settings.contains("dp/showF0"))
        this->ui->showF0->setChecked(settings.value("dp/showF0").toBool());
    if(settings.contains("dp/showOriginalF0"))
        this->ui->showOriginalF0->setChecked(settings.value("dp/showOriginalF0").toBool());
    if(settings.contains("dp/showTime"))
        this->ui->showTime->setChecked(settings.value("dp/showTime").toBool());
    if(settings.contains("error/type"))
        this->ui->errorTypeBox->setCurrentIndex(settings.value("error/type").toInt());
    if(settings.contains("dp/use"))
        this->ui->useForDP->setCurrentIndex(settings.value("dp/use").toInt());
    if(settings.contains("dp/recordingSeconds"))
        this->ui->recordingSeconds->setValue(settings.value("dp/recordingSeconds").toInt());
    if(settings.contains("dp/recordingType"))
        this->ui->recordingType->setCurrentIndex(settings.value("dp/recordingType").toInt());


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
    if(settings.contains("lpc/cepstrum_order"))
        this->ui->lpcCepstrumOrderSpin->setValue(settings.value("lpc/cepstrum_order").toInt());
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
//    settings.setValue("plot_f0/interpolation_edges", this->ui->plotInterpolationEdgesF0Spin->value());
    settings.setValue("plot_f0/interpolation_type", this->ui->plotInterpolationTypeF0Box->currentIndex());
    settings.setValue("plot_f0/normF0MinMax", this->ui->normF0MinMax->isChecked());

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
    settings.setValue("lpc/cepstrum_order", this->ui->lpcCepstrumOrderSpin->value());
    settings.setValue("spec/leng", this->ui->frameSizeBox->currentText().toInt());
    settings.setValue("spec/order", this->ui->lpcOrderSpin->value());

    settings.setValue("spec/factor", this->ui->specFactorSpin->value());
    settings.setValue("spec/min", this->ui->specMinSpin->value());
    settings.setValue("spec/proc", this->ui->specProcBox->currentText());

    settings.setValue("move/type", this->ui->moveTypeBox->currentText());

    settings.setValue("dp/global", this->ui->dpGlobalLimit->value());
    settings.setValue("dp/local", this->ui->dpLocalLimit->value());
    settings.setValue("dp/continiusLimit", this->ui->continiusDpLimit->value());

    settings.setValue("dp/continiusDpKD", this->ui->continiusDpKD->value());
    settings.setValue("dp/continiusDpKH", this->ui->continiusDpKH->value());
    settings.setValue("dp/continiusDpKT", this->ui->continiusDpKT->value());
    settings.setValue("dp/continiusDpKV", this->ui->continiusDpKV->value());

    settings.setValue("dp/portLen", this->ui->portLen->value());
    settings.setValue("dp/useStripUmp", this->ui->useStripUmp->isChecked());
    settings.setValue("dp/showA0", this->ui->showA0->isChecked());
    settings.setValue("dp/showPortr", this->ui->showPortr->isChecked());
    settings.setValue("dp/showError", this->ui->showError->isChecked());
    settings.setValue("dp/showF0", this->ui->showF0->isChecked());
    settings.setValue("dp/showOriginalF0", this->ui->showOriginalF0->isChecked());
    settings.setValue("dp/showTime", this->ui->showTime->isChecked());
    settings.setValue("error/type", this->ui->errorTypeBox->currentIndex());
    settings.setValue("dp/use", this->ui->useForDP->currentIndex());
    settings.setValue("dp/recordingType", this->ui->recordingType->currentIndex());
    settings.setValue("dp/recordingSeconds", this->ui->recordingSeconds->value());

    settings.setValue("mathGL/quality", this->ui->mathGLQualitySpin->value());
    settings.setValue("mathGL/autoOpen", this->ui->isAutoOpen->isChecked());
}
