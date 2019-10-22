#include "webapi.h"
#include "qdebug.h"

#include <cmath>

#include "utils.h"
#include "defines.h"

#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QUuid>
#include <QVariantList>

RecordFile::RecordFile(QObject *parent) : QObject(parent)
{
    this->isEmpty = true;
    this->data_auto_created = false;
    this->data_manual_created = false;
}

RecordFile::RecordFile(QString title, QString text, QString path, QObject *parent) : QObject(parent)
{
    this->path = QApplication::applicationDirPath() + DATA_PATH + path;
    this->isEmpty = false;
    this->data_auto_created = false;
    this->data_manual_created = false;
    this->title = title;
    this->text = text;
    this->uuid = QUuid::createUuid().toString();
}

QJsonObject RecordFile::toJson()
{
    QJsonObject jItem;
    jItem["title"] = this->title;
    jItem["text"] = this->text;
    jItem["path"] = this->path;
    jItem["uuid"] = this->uuid;
    return jItem;
}

SimpleGraphData * RecordFile::getData(bool manual)
{
    qDebug() << "RecordFile::getData " << manual << " " << this->uuid << LOG_DATA;
    if (manual)
    {
        qDebug() << "RecordFile::getData data_manual " << this->data_manual_created;
        if (!this->data_manual_created) {
            qDebug() << "RecordFile::getData create data_manual";
            this->data_manual = SimpleProcWave2Data(this->path, manual);
            this->data_manual_created = true;
        }
        return this->data_manual;
    } else
    {
        qDebug() << "RecordFile::getData data_auto " << this->data_auto_created;
        if (!this->data_auto_created) {
            qDebug() << "RecordFile::getData create data_auto";
            this->data_auto = SimpleProcWave2Data(this->path, manual);
            this->data_auto_created = true;
        }
        return this->data_auto;
    }
}

WebApi::WebApi(QObject *parent) : QObject(parent)
{

}

QString WebApi::getRecords()
{
    QStringList files = scanDirItems(QApplication::applicationDirPath() + DATA_PATH_TRAINING, WAVE_TYPE, QApplication::applicationDirPath() + DATA_PATH);
    files.sort();
    QString lastPath = "";

    QJsonArray jResult;
    QJsonObject jSection;
    QJsonArray jList;
    for(int i=0; i<files.size();i++)
    {
        QString file = files.at(i);
        int lastIndex = file.lastIndexOf("/");
        int firstIndex = file.indexOf("/", 1);

        QString filePath = file.left(lastIndex).replace("/", " ");
        QString fileTitle = filePath.mid(firstIndex).replace("/", " ");
        QString fileName = file.mid(lastIndex).replace("/", " ");

        if (lastPath == "") lastPath = fileTitle;

        if (lastPath != fileTitle)
        {
            jSection["title"] = lastPath;
            jSection["uuid"] = QUuid::createUuid().toString();
            jSection["files"] = jList;
            jResult.append(jSection);

            QJsonObject jNewSection;
            jSection = jNewSection;
            lastPath = fileTitle;
            QJsonArray jNewList;
            jList = jNewList;
        }

        RecordFile* recordFile = new RecordFile(fileName.remove(".wav"), fileName, file, this);
        if (!this->files.contains(recordFile->uuid))
        {
            this->files.insert(recordFile->uuid, recordFile);
        }
        jList.append(recordFile->toJson());
    }
    jSection["title"] = lastPath;
    jSection["files"] = jList;
    jResult.append(jSection);

    QJsonDocument jDoc(jResult);

//    qDebug() << "getFiles " << QString(jDoc.toJson(QJsonDocument::Compact)) << LOG_DATA;

    return QString(jDoc.toJson(QJsonDocument::Compact));
}

QString WebApi::getRecordInfo(QString uuid)
{
    qDebug() << "WebApi::getRecordInfo " << uuid << LOG_DATA;
    RecordFile* recordFile = this->files.value(uuid);
    if (recordFile->isEmpty) return QString(ERROR_NOT_FOUND);
    QJsonDocument jDoc(recordFile->toJson());
    return QString(jDoc.toJson(QJsonDocument::Compact));
}

QVariantList vectorToVariantList(vector vec)
{
    QVariantList list;
    for (int i=0; i<vec.x; i++)
    {
        list.insert(i, QVariant(getv(vec, i)));
    }
    return list;
}

QString WebApi::getRecordWave(QString uuid)
{
    qDebug() << "WebApi::getRecordWave " << uuid << LOG_DATA;
    RecordFile* recordFile = this->files.value(uuid);
    if (recordFile->isEmpty) return QString(ERROR_NOT_FOUND);

    SimpleGraphData * data = recordFile->getData(false);
    vector vec = data_get_full_wave(data);
    QVariantList list = vectorToVariantList(vec);
    QJsonDocument jDoc(QJsonArray::fromVariantList(list));
    return QString(jDoc.toJson(QJsonDocument::Compact));
}

QString WebApi::getRecordPitch(QString uuid, bool manual)
{
    qDebug() << "WebApi::getRecordPitch " << manual << " " << uuid << LOG_DATA;
    RecordFile* recordFile = this->files.value(uuid);
    if (recordFile->isEmpty) return QString(ERROR_NOT_FOUND);

    SimpleGraphData * data = recordFile->getData(manual);
    vector vec = data_get_pitch_smooth(data);
    QVariantList list = vectorToVariantList(vec);
    QJsonDocument jDoc(QJsonArray::fromVariantList(list));
    return QString(jDoc.toJson(QJsonDocument::Compact));
}

QString WebApi::getRecordOctavesRange(QString uuid, bool manual)
{
    qDebug() << "WebApi::getRecordOctavesRange " << manual << " " << uuid << LOG_DATA;
    RecordFile* recordFile = this->files.value(uuid);
    if (recordFile->isEmpty) return QString(ERROR_NOT_FOUND);

    SimpleGraphData * data = recordFile->getData(manual);
    double min = data_get_pitch_min(data);
    double max = data_get_pitch_max(data);
    double value = (max / min) - 1.0;
    double range = ceil(value / OCTAVE_RANGE) * OCTAVE_RANGE;

    QJsonObject js_root;
    js_root["min"] = min;
    js_root["max"] = max;
    js_root["value"] = value;
    js_root["range"] = range;
    QJsonDocument jDoc(js_root);

    return QString(jDoc.toJson(QJsonDocument::Compact));
}

QJsonDocument maskToJson(MaskData mask)
{
    QJsonObject js_root;
    js_root["length"] = mask.length;
    QJsonArray js_list;
    for (int i=0; i<mask.pointsFrom.x && i<mask.pointsLength.x; i++) {
        QJsonObject js_item;
        js_item["position"] = mask.pointsFrom.v[i];
        js_item["length"] = mask.pointsLength.v[i];
        js_list.append(js_item);
    }
    js_root["data"] = js_list;

    qDebug() << "maskToJson res" << mask.pointsFrom.x << LOG_DATA;
    QJsonDocument jDoc(js_root);
    return jDoc;
}

QString WebApi::getSegmentsP(QString uuid, bool manual)
{
    qDebug() << "WebApi::getSegmentsP " << manual << " " << uuid << LOG_DATA;
    RecordFile* recordFile = this->files.value(uuid);
    if (recordFile->isEmpty) return QString(ERROR_NOT_FOUND);

    SimpleGraphData * data = recordFile->getData(manual);
    MaskData mask = data_get_p(data);
    QJsonDocument jDoc = maskToJson(mask);
    return QString(jDoc.toJson(QJsonDocument::Compact));
}

QString WebApi::getSegmentsT(QString uuid, bool manual)
{
    qDebug() << "WebApi::getSegmentsT " << manual << " " << uuid << LOG_DATA;
    RecordFile* recordFile = this->files.value(uuid);
    if (recordFile->isEmpty) return QString(ERROR_NOT_FOUND);

    SimpleGraphData * data = recordFile->getData(manual);
    MaskData mask = data_get_t(data);
    QJsonDocument jDoc = maskToJson(mask);
    return QString(jDoc.toJson(QJsonDocument::Compact));
}

QString WebApi::getSegmentsN(QString uuid, bool manual)
{
    qDebug() << "WebApi::getSegmentsN " << manual << " " << uuid << LOG_DATA;
    RecordFile* recordFile = this->files.value(uuid);
    if (recordFile->isEmpty) return QString(ERROR_NOT_FOUND);

    SimpleGraphData * data = recordFile->getData(manual);
    MaskData mask = data_get_n(data);
    QJsonDocument jDoc = maskToJson(mask);
    return QString(jDoc.toJson(QJsonDocument::Compact));
}

QString WebApi::getUPM(QString uuid, bool manual)
{
    qDebug() << "WebApi::getUPM " << manual << " " << uuid << LOG_DATA;
    RecordFile* recordFile = this->files.value(uuid);
    if (recordFile->isEmpty) return QString(ERROR_NOT_FOUND);

    SimpleGraphData * data = recordFile->getData(manual);

    vector vec = data_get_ump(data, manual);

    QVariantList list = vectorToVariantList(vec);
    QJsonDocument jDoc(QJsonArray::fromVariantList(list));
    return QString(jDoc.toJson(QJsonDocument::Compact));
}

QString WebApi::getUPMMask(QString uuid, bool manual)
{
    qDebug() << "WebApi::getUPMMask " << manual << " " << uuid << LOG_DATA;
    RecordFile* recordFile = this->files.value(uuid);
    if (recordFile->isEmpty) return QString(ERROR_NOT_FOUND);

    SimpleGraphData * data = recordFile->getData(manual);
    MaskData mask = data_get_ump_mask(data, manual);
    QJsonDocument jDoc = maskToJson(mask);
    return QString(jDoc.toJson(QJsonDocument::Compact));
}
