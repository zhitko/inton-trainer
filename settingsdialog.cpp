#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "databaseeditor/databasemanager.h"

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

    this->databaseManager = new DatabaseManager(ui->databaseView, this);
    connect(this->databaseManager, SIGNAL(activeFileChanged(QString)), this->ui->activeFile, SLOT(setText(QString)));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
//    if(currentOutputDevice) freeAudioOutputDevice(currentOutputDevice);
}

SettingsDialog * SettingsDialog::getInstance(QWidget *parent)
{
    static SettingsDialog * instance = new SettingsDialog(parent);
    return instance;
}

void SettingsDialog::initAudio()
{
    this->inputDevices = getInputDevices();
    this->outputDevices = getOutputDevices();

    if(this->inputDevices) this->currentInputDevice = this->inputDevices->device;
    if(this->outputDevices) this->currentOutputDevice = this->outputDevices->device;
    if(this->currentInputDevice) qDebug() << "currentInputDevice " << this->currentInputDevice->name << LOG_DATA;
    if(this->currentOutputDevice) qDebug() << "currentOutputDevice " << this->currentOutputDevice->name << LOG_DATA;
}

void SettingsDialog::inputDeviceChanged(int index)
{
    if (!this->inputDevices) return;

    oal_devices_list *list = this->inputDevices;
    for(int i=0;i<index;i++) list = list->next;
    this->currentInputDevice = list->device;
    qDebug() << "inputDeviceChanged " << this->currentInputDevice->name << LOG_DATA;
}

void SettingsDialog::outputDeviceChanged(int index)
{
    if (!this->outputDevices) return;

    oal_devices_list *list = this->outputDevices;
    for(int i=0;i<index;i++) list = list->next;
    this->currentOutputDevice = list->device;
    qDebug() << "outputDeviceChanged " << this->currentOutputDevice->name << LOG_DATA;
}

void SettingsDialog::initUI()
{
    if (this->inputDevices)
    {
        oal_devices_list *list = this->inputDevices;
        while(list)
        {
            QString name = list->device->name;
            this->ui->audioInputDeviceBox->addItem(name);
            list = list->next;
        }
        connect(this->ui->audioInputDeviceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(inputDeviceChanged(int)));
    }

    if (this->outputDevices)
    {
        oal_devices_list *list = this->outputDevices;
        while(list)
        {
            QString name = list->device->name;
            this->ui->audioOutputDeviceBox->addItem(name);
            list = list->next;
        }
        connect(this->ui->audioOutputDeviceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(outputDeviceChanged(int)));
    }

    connect(this->ui->buttonBox, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(this->ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttons(QAbstractButton*)));
}

void SettingsDialog::buttons(QAbstractButton *button)
{
    if (this->ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole)
        setDefaultSettings();
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

    sptk_settings->pitch->MAX_FREQ = instance->ui->pitchMaxFreqSpin->value();
    sptk_settings->pitch->MIN_FREQ = instance->ui->pitchMinFreqSpin->value();
    sptk_settings->pitch->FRAME_SHIFT = instance->ui->pitchFrameShoftSpin->value();
    sptk_settings->pitch->ATYPE = instance->ui->pitchATypeSpin->value();
    sptk_settings->pitch->OTYPE = instance->ui->pitchOTypeSpin->value();
    sptk_settings->pitch->THRESH_RAPT = instance->ui->pitchThreshRaptSpin->value();
    sptk_settings->pitch->THRESH_SWIPE = instance->ui->pitchThreshSwipeSpin->value();

    sptk_settings->plotF0->frame = instance->ui->plotMidFrameF0Spin->value();
    sptk_settings->plotF0->interpolation_type = instance->ui->plotInterpolationTypeF0Box->currentIndex();
    sptk_settings->plotF0->normF0MinMax = instance->ui->normF0MinMax->isChecked();

    sptk_settings->plotEnergy->frame = instance->ui->plotLinFrameEnergySpin->value();
    sptk_settings->plotEnergy->interpolation_type = instance->ui->plotInterpolationTypeEnergyBox->currentIndex();

    sptk_settings->frame->leng = instance->ui->frameSizeBox->currentText().toInt();
    sptk_settings->frame->shift = instance->ui->frameShiftSpin->value();
    sptk_settings->window->leng = instance->ui->frameSizeBox->currentText().toInt();
    sptk_settings->window->window_type = static_cast<Window>(instance->ui->windoTypeBox->currentIndex());
    sptk_settings->lpc->leng = instance->ui->frameSizeBox->currentText().toInt();
    sptk_settings->spec->leng = instance->ui->frameSizeBox->currentText().toInt();

    sptk_settings->dp->continiusLimit = instance->ui->continiusDpLimit->value();

    sptk_settings->dp->continiusKD = instance->ui->continiusDpKD->value();
    sptk_settings->dp->continiusKH = instance->ui->continiusDpKH->value();
    sptk_settings->dp->continiusKT = instance->ui->continiusDpKT->value();
    sptk_settings->dp->continiusKV = instance->ui->continiusDpKV->value();

    sptk_settings->dp->portLen = instance->ui->portLen->value();
    sptk_settings->dp->umpSmoothType = instance->ui->umpSmoothType->currentIndex();
    sptk_settings->dp->umpSmoothValue = instance->ui->umpSmoothValue->value();
    sptk_settings->dp->showA0 = instance->ui->showA0->isChecked();
    sptk_settings->dp->showPortr = instance->ui->showPortr->isChecked();
    sptk_settings->dp->showError = instance->ui->showError->isChecked();
    sptk_settings->dp->showF0 = instance->ui->showF0->isChecked();
    sptk_settings->dp->showOriginalF0 = instance->ui->showOriginalF0->isChecked();
    sptk_settings->dp->showDerivativeF0 = instance->ui->showDerivativeF0->isChecked();
    sptk_settings->dp->showPlane = instance->ui->showPlane->isChecked();
    sptk_settings->dp->showTime = instance->ui->showTime->isChecked();
    sptk_settings->dp->errorType = instance->ui->errorTypeBox->currentIndex();
    sptk_settings->dp->useForDP = instance->ui->useForDP->currentIndex();
    sptk_settings->dp->dpA0Coeficient = instance->ui->dpA0Coeficient->value();
    sptk_settings->dp->dpDA0Coeficient = instance->ui->dpDA0Coeficient->value();
    sptk_settings->dp->showMeanValueUMP = instance->ui->showMeanValueUMP->isChecked();
    sptk_settings->dp->showCenterGravityUMP = instance->ui->showCenterGravityUMP->isChecked();

    sptk_settings->dp->recordingSeconds = instance->ui->recordingSeconds->value();
    sptk_settings->dp->recordingMaxFiles = instance->ui->recordingMaxFiles->value();

    sptk_settings->dp->mark_delimeter = instance->ui->markDelimeter->value();
    sptk_settings->dp->mark_level = instance->ui->markLimit->value();
    sptk_settings->dp->relative_limit = instance->ui->relativeLimit->value();
    sptk_settings->dp->mark_labels = strdup(instance->ui->markLabels->text().toStdString().c_str());

    sptk_settings->dp->markoutA0limit = instance->ui->markoutA0limit->value();

    sptk_settings->dp->show_marks = instance->ui->showMarks->isChecked();
    sptk_settings->dp->auto_marking = instance->ui->autoMarking->isChecked();

    return sptk_settings;
}

MathGLSettings * SettingsDialog::getMathGLSettings()
{
    MathGLSettings * settings = new MathGLSettings();
    settings->quality = 5;
    settings->autoOpen = true;
    return settings;
}

void SettingsDialog::setDefaultSettings()
{
    qDebug() << "loadSettings " << LOG_DATA;
    this->loadSettingsFrom(SETTINGS_DEFAULT_FILE);
    this->saveSettings();
}

void SettingsDialog::loadSettings()
{
    qDebug() << "loadSettings " << LOG_DATA;
    this->loadSettingsFrom(SETTINGS_FILE);
}

void SettingsDialog::loadSettingsFrom(QString settings_path)
{
    qDebug() << "loadSettingsFrom " << settings_path << LOG_DATA;
    QSettings settings(settings_path, QSettings::IniFormat);
    settings.setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());

    if(settings.contains("pitch/max_freq"))
        this->ui->pitchMaxFreqSpin->setValue(settings.value("pitch/max_freq").toInt());
    if(settings.contains("pitch/min_freq"))
        this->ui->pitchMinFreqSpin->setValue(settings.value("pitch/min_freq").toInt());

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
    if(settings.contains("plot_f0/normF0MinMax"))
        this->ui->normF0MinMax->setChecked(settings.value("plot_f0/normF0MinMax").toBool());

    if(settings.contains("plot_energy/mid_frame"))
        this->ui->plotLinFrameEnergySpin->setValue(settings.value("plot_energy/frame").toInt());
    if(settings.contains("plot_energy/interpolation_type"))
        this->ui->plotInterpolationTypeEnergyBox->setCurrentIndex(settings.value("plot_energy/interpolation_type").toInt());

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
    if(settings.contains("dp/umpSmoothValue"))
        this->ui->umpSmoothValue->setValue(settings.value("dp/umpSmoothValue").toInt());
    if(settings.contains("dp/umpSmoothType"))
        this->ui->umpSmoothType->setCurrentIndex(settings.value("dp/umpSmoothType").toInt());
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
    if(settings.contains("dp/showDerivativeF0"))
        this->ui->showDerivativeF0->setChecked(settings.value("dp/showDerivativeF0").toBool());
    if(settings.contains("dp/showPlane"))
        this->ui->showPlane->setChecked(settings.value("dp/showPlane").toBool());
    if(settings.contains("dp/showTime"))
        this->ui->showTime->setChecked(settings.value("dp/showTime").toBool());
    if(settings.contains("error/type"))
        this->ui->errorTypeBox->setCurrentIndex(settings.value("error/type").toInt());
    if(settings.contains("dp/use"))
        this->ui->useForDP->setCurrentIndex(settings.value("dp/use").toInt());
    if(settings.contains("dp/a0Coeficient"))
        this->ui->dpA0Coeficient->setValue(settings.value("dp/a0Coeficient").toDouble());
    if(settings.contains("dp/DA0Coeficient"))
        this->ui->dpDA0Coeficient->setValue(settings.value("dp/DA0Coeficient").toDouble());
    if(settings.contains("dp/recordingSeconds"))
        this->ui->recordingSeconds->setValue(settings.value("dp/recordingSeconds").toDouble());
    if(settings.contains("dp/recordingMaxFiles"))
        this->ui->recordingMaxFiles->setValue(settings.value("dp/recordingMaxFiles").toInt());
    if(settings.contains("dp/showMeanValueUMP"))
        this->ui->showMeanValueUMP->setChecked(settings.value("dp/showMeanValueUMP").toBool());
    if(settings.contains("dp/showCenterGravityUMP"))
        this->ui->showCenterGravityUMP->setChecked(settings.value("dp/showCenterGravityUMP").toBool());

    if(settings.contains("frame/leng"))
        this->ui->frameSizeBox->setCurrentText(QString::number(settings.value("frame/leng").toInt()));
    if(settings.contains("frame/shift"))
        this->ui->frameShiftSpin->setValue(settings.value("frame/shift").toInt());
    if(settings.contains("window/window_type"))
        this->ui->windoTypeBox->setCurrentText(settings.value("window/window_type").toString());

    if(settings.contains("dp/markDelimeter"))
        this->ui->markDelimeter->setValue(settings.value("dp/markDelimeter").toDouble());
    if(settings.contains("dp/markLimit"))
        this->ui->markLimit->setValue(settings.value("dp/markLimit").toDouble());
    if(settings.contains("dp/relativeLimit"))
        this->ui->relativeLimit->setValue(settings.value("dp/relativeLimit").toDouble());
    if(settings.contains("dp/markLabels"))
        this->ui->markLabels->setText(settings.value("dp/markLabels").toString());

    if(settings.contains("dp/showMarks"))
        this->ui->showMarks->setChecked(settings.value("dp/showMarks").toBool());
    if(settings.contains("dp/autoMarking"))
        this->ui->autoMarking->setChecked(settings.value("dp/autoMarking").toBool());

    if(settings.contains("dp/markoutA0limit"))
        this->ui->markoutA0limit->setValue(settings.value("dp/markoutA0limit").toInt());

}

void SettingsDialog::saveSettings()
{
    qDebug() << "saveSettings to " << SETTINGS_FILE << LOG_DATA;
    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
    settings.setPath(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationDirPath());

    settings.setValue("pitch/max_freq", this->ui->pitchMaxFreqSpin->value());
    settings.setValue("pitch/min_freq", this->ui->pitchMinFreqSpin->value());

    settings.setValue("pitch/FRAME_SHIFT", this->ui->pitchFrameShoftSpin->value());
    settings.setValue("pitch/ATYPE", this->ui->pitchATypeSpin->value());
    settings.setValue("pitch/OTYPE", this->ui->pitchOTypeSpin->value());
    settings.setValue("pitch/THRESH_RAPT", this->ui->pitchThreshRaptSpin->value());
    settings.setValue("pitch/THRESH_SWIPE", this->ui->pitchThreshSwipeSpin->value());

    settings.setValue("plot_f0/mid_frame", this->ui->plotMidFrameF0Spin->value());
    settings.setValue("plot_f0/interpolation_type", this->ui->plotInterpolationTypeF0Box->currentIndex());
    settings.setValue("plot_f0/normF0MinMax", this->ui->normF0MinMax->isChecked());

    settings.setValue("plot_energy/frame", this->ui->plotLinFrameEnergySpin->value());
    settings.setValue("plot_energy/interpolation_type", this->ui->plotInterpolationTypeEnergyBox->currentIndex());

    settings.setValue("frame/leng", this->ui->frameSizeBox->currentText().toInt());
    settings.setValue("frame/shift", this->ui->frameShiftSpin->value());
    settings.setValue("window/leng", this->ui->frameSizeBox->currentText().toInt());
    settings.setValue("window/window_type", this->ui->windoTypeBox->currentText());
    settings.setValue("lpc/leng", this->ui->frameSizeBox->currentText().toInt());
    settings.setValue("spec/leng", this->ui->frameSizeBox->currentText().toInt());

    settings.setValue("dp/continiusLimit", this->ui->continiusDpLimit->value());

    settings.setValue("dp/continiusDpKD", this->ui->continiusDpKD->value());
    settings.setValue("dp/continiusDpKH", this->ui->continiusDpKH->value());
    settings.setValue("dp/continiusDpKT", this->ui->continiusDpKT->value());
    settings.setValue("dp/continiusDpKV", this->ui->continiusDpKV->value());

    settings.setValue("dp/portLen", this->ui->portLen->value());
    settings.setValue("dp/umpSmoothType", this->ui->umpSmoothType->currentIndex());
    settings.setValue("dp/umpSmoothValue", this->ui->umpSmoothValue->value());
    settings.setValue("dp/showA0", this->ui->showA0->isChecked());
    settings.setValue("dp/showPortr", this->ui->showPortr->isChecked());
    settings.setValue("dp/showError", this->ui->showError->isChecked());
    settings.setValue("dp/showF0", this->ui->showF0->isChecked());
    settings.setValue("dp/showOriginalF0", this->ui->showOriginalF0->isChecked());
    settings.setValue("dp/showDerivativeF0", this->ui->showDerivativeF0->isChecked());
    settings.setValue("dp/showPlane", this->ui->showPlane->isChecked());
    settings.setValue("dp/showTime", this->ui->showTime->isChecked());
    settings.setValue("error/type", this->ui->errorTypeBox->currentIndex());
    settings.setValue("dp/use", this->ui->useForDP->currentIndex());
    settings.setValue("dp/a0Coeficient", this->ui->dpA0Coeficient->value());
    settings.setValue("dp/DA0Coeficient", this->ui->dpDA0Coeficient->value());
    settings.setValue("dp/recordingSeconds", this->ui->recordingSeconds->value());
    settings.setValue("dp/recordingMaxFiles", this->ui->recordingMaxFiles->value());
    settings.setValue("dp/showMeanValueUMP", this->ui->showMeanValueUMP->isChecked());
    settings.setValue("dp/showCenterGravityUMP", this->ui->showCenterGravityUMP->isChecked());

    settings.setValue("dp/markDelimeter", this->ui->markDelimeter->value());
    settings.setValue("dp/markLimit", this->ui->markLimit->value());
    settings.setValue("dp/relativeLimit", this->ui->relativeLimit->value());
    settings.setValue("dp/markLabels", this->ui->markLabels->text());

    settings.setValue("dp/showMarks", this->ui->showMarks->isChecked());
    settings.setValue("dp/autoMarking", this->ui->autoMarking->isChecked());

    settings.setValue("dp/markoutA0limit", this->ui->markoutA0limit->value());
}

void SettingsDialog::on_showPlane_stateChanged(int arg1)
{
    if (arg1)
    {
        this->ui->showDerivativeF0->setChecked(arg1);
        this->ui->showF0->setChecked(arg1);
    }
}

void SettingsDialog::on_showF0_stateChanged(int arg1)
{
    if (!arg1)
    {
        this->ui->showPlane->setChecked(arg1);
    }
}

void SettingsDialog::on_showDerivativeF0_stateChanged(int arg1)
{
    if (!arg1)
    {
        this->ui->showPlane->setChecked(arg1);
    }
}
