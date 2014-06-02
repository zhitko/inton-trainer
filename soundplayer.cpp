#include "soundplayer.h"

#include <QDebug>
#include <QTextCodec>
#include <QFile>

#include "settingsdialog.h"

SoundPlayer::SoundPlayer(QString path, QObject *parent) :
    QThread(parent),
    path(path)
{
    qDebug() << "SoundPlayer: init";
    this->device = SettingsDialog::getInstance()->getOutputDevice();
    qDebug() << "SoundPlayer: init output device " << QString::fromLocal8Bit(this->device->name);
    initAudioOutputDevice(this->device);
    if(!isAudioOk){
        qDebug() << "SoundPlayer: FAIL init output device " << QString::fromLocal8Bit(this->device->name);
        return;
    }
    qDebug() << "SoundPlayer: open wav file " << path;

    QFile file(path);
    file.open(QIODevice::ReadOnly);
    this->waveFile = waveOpenHFile(file.handle());
}

SoundPlayer::~SoundPlayer()
{
    qDebug() << "~SoundPlayer";
    freeAudioOutputDevice(this->device);
    waveCloseFile(waveFile);
}

void SoundPlayer::run() Q_DECL_OVERRIDE
{
    qDebug() << "SoundPlayer: run";
    playSound(
                this->device,
                this->waveFile->dataChunk->waveformData,
                littleEndianBytesToUInt32(this->waveFile->dataChunk->chunkDataSize),
                littleEndianBytesToUInt16(this->waveFile->formatChunk->numberOfChannels),
                littleEndianBytesToUInt16(this->waveFile->formatChunk->significantBitsPerSample),
                littleEndianBytesToUInt32(this->waveFile->formatChunk->sampleRate));
    this->deleteLater();
}
