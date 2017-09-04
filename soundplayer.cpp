#include "soundplayer.h"

#include <QDebug>
#include <QTextCodec>
#include <QFile>

#include "settingsdialog.h"

SoundPlayer::SoundPlayer(QString path, QObject *parent) :
    QThread(parent),
    path(path)
{
    qDebug() << "SoundPlayer: init" << LOG_DATA;
    this->device = SettingsDialog::getInstance()->getOutputDevice();
    if (!this->device) return;

    qDebug() << "SoundPlayer: init output device " << QString::fromLocal8Bit(this->device->name) << LOG_DATA;
    initAudioOutputDevice(this->device);
    if(!isAudioOk){
        qDebug() << "SoundPlayer: FAIL init output device " << QString::fromLocal8Bit(this->device->name) << LOG_DATA;
        return;
    }
    qDebug() << "SoundPlayer: open wav file " << path << LOG_DATA;

    QFile file(path);
    file.open(QIODevice::ReadOnly);
    this->waveFile = waveOpenHFile(file.handle());
}

SoundPlayer::~SoundPlayer()
{
    qDebug() << "~SoundPlayer" << LOG_DATA;
    freeAudioOutputDevice(this->device);
    waveCloseFile(waveFile);
}

void SoundPlayer::run()
{
    qDebug() << "SoundPlayer: run" << LOG_DATA;
    playSound(
                this->device,
                this->waveFile->dataChunk->waveformData,
                littleEndianBytesToUInt32(this->waveFile->dataChunk->chunkDataSize),
                littleEndianBytesToUInt16(this->waveFile->formatChunk->numberOfChannels),
                littleEndianBytesToUInt16(this->waveFile->formatChunk->significantBitsPerSample),
                littleEndianBytesToUInt32(this->waveFile->formatChunk->sampleRate));
    emit finished();
    this->deleteLater();
}
