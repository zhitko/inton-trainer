#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "defines.h"

#include <QDialog>
#include <QAbstractButton>

#define SETTINGS_FILE "settings.conf"
#define SETTINGS_DEFAULT_FILE "settings.default"

extern "C" {
    #include "./OpenAL/openal_wrapper.h"
    #include "./SPTK.h"
}

namespace Ui {
    class SettingsDialog;
}

typedef struct _MathGLSettings
{
    int quality;
    bool autoOpen;
} MathGLSettings;

class DatabaseManager;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    ~SettingsDialog();
    static SettingsDialog * getInstance(QWidget *parent = 0);

private:
    void loadSettingsFrom(QString);
    explicit SettingsDialog(QWidget *parent = 0);
    Ui::SettingsDialog *ui;

    void initAudio();
    void initUI();

    oal_devices_list *inputDevices, *outputDevices;
    oal_device *currentInputDevice, *currentOutputDevice;

    DatabaseManager * databaseManager;

public:
    void loadSettings();
    oal_device * getInputDevice();
    oal_device * getOutputDevice();
    static SPTK_SETTINGS * getSPTKsettings(bool force = false);
    static MathGLSettings * getMathGLSettings();

private slots:
    void buttons(QAbstractButton*);
    void inputDeviceChanged(int);
    void outputDeviceChanged(int);
    void saveSettings();
    void setDefaultSettings();
    void on_showPlane_stateChanged(int arg1);
    void on_showF0_stateChanged(int arg1);
    void on_showDerivativeF0_stateChanged(int arg1);
    void on_markoutType_currentIndexChanged(int index);
    void on_checkBox_8_stateChanged(int arg1);
    void on_autoMarking_stateChanged(int arg1);
};

#endif // SETTINGSDIALOG_H
