#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

#define SETTINGS_FILE "settings.conf"

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
    bool realtime;
} MathGLSettings;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    ~SettingsDialog();
    static SettingsDialog * getInstance(QWidget *parent = 0);

private:
    explicit SettingsDialog(QWidget *parent = 0);
    Ui::SettingsDialog *ui;

    void loadSettings();
    void initAudio();
    void initUI();

    oal_devices_list *inputDevices, *outputDevices;
    oal_device *currentInputDevice, *currentOutputDevice;

public:
    oal_device * getInputDevice();
    oal_device * getOutputDevice();
    static SPTK_SETTINGS * getSPTKsettings();
    static MathGLSettings * getMathGLSettings();

private slots:
    void inputDeviceChanged(int);
    void outputDeviceChanged(int);
    void saveSettings();
};

#endif // SETTINGSDIALOG_H
