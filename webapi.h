#ifndef WEBAPI_H
#define WEBAPI_H

#include <QObject>
#include <QHash>

#include "dataprocessing.h"

#define ERROR_NOT_FOUND "{'error':'Not found'}"

class RecordFile : public QObject
{
    Q_OBJECT
public:
    explicit RecordFile(QObject *parent = nullptr);
    explicit RecordFile(QString title, QString text, QString path, QObject *parent = nullptr);

    bool isEmpty;

    QString title;
    QString uuid;
    QString text;
    QString path;

    QJsonObject toJson();
    SimpleGraphData * getData(bool manual);

private:
    SimpleGraphData *data_manual;
    bool data_manual_created;
    SimpleGraphData *data_auto;
    bool data_auto_created;
};

class WebApi : public QObject
{
    Q_OBJECT
public:
    explicit WebApi(QObject *parent = nullptr);

signals:

public slots:
    QString getRecords();
    QString getRecordInfo(QString uuid);
    QString getRecordWave(QString uuid);
    QString getRecordPitch(QString uuid, bool manual);
    QString getRecordOctavesRange(QString uuid, bool manual);
    QString getSegmentsP(QString uuid, bool manual);
    QString getSegmentsT(QString uuid, bool manual);
    QString getSegmentsN(QString uuid, bool manual);
    QString getUPM(QString uuid, bool manual);
    QString getUPMMask(QString uuid, bool manual);

private:
    QHash<QString, RecordFile*> files;
};

#endif // WEBAPI_H
