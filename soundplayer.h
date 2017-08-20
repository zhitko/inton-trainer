#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include "defines.h"

#include <QThread>

extern "C" {
    #include "./OpenAL/wavFile.h"
    #include "./OpenAL/openal_wrapper.h"
}

class SoundPlayer : public QThread
{
    Q_OBJECT
protected:
    void run();
public:
    explicit SoundPlayer(QString path, QObject *parent = 0);
    ~SoundPlayer();
private:
    QString path;
    oal_device * device;
    WaveFile * waveFile;
signals:

public slots:

};

#endif // SOUNDPLAYER_H
