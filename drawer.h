#ifndef DRAWER_H
#define DRAWER_H

#include <QMap>

#include "defines.h"

#include <mgl2/qmathgl.h>

#define GRAPH_Y_VAL_MAX 10000

#include "DP/vectordp.h"
#include "DP/spectrdp.h"
#include "dataprocessing.h"

extern "C" {
    #include "analysis/metrics.h"
    #include "./OpenAL/wavFile.h"
    #include "./SPTK/SPTK.h"
    #include "./SPTK/vector.h"
}

mglData * createMglData(vector vec, mglData * data, bool nan = false);
void vectorToData(vector vec, mglData * data);
void vectorToDataWithNan(vector vec, mglData * data);
vector scaleVectorByDPResults(vector, SpectrDP*);
vector getSignalWithMask(vector, SpectrDP*, vector);

class Drawer: public mglDraw
{
public:
    Drawer();
    virtual ~Drawer();

    bool stereo;
    bool showUMP;
    int recordNumber = 0;

    virtual int getDataLenght();
    virtual int getDataSeconds();

    virtual int Draw(mglGraph *gr);
    virtual void Proc(QString fname, bool ref = false);
    virtual void reProc();
    double calcResultMark(vector, vector);

    void Reload();
    void Click();

    double proximity_curve_correlation, proximity_curve_integral, proximity_curve_local, proximity_average;
    double proximity_range, proximity_curve_shape;

    MetricsData getMetricData();

protected:
    GraphData * data;
    QString fileName;
    mglData *waveData, *pitchData, *pitchDataOriginal, *pitchDataDerivative, *specData, *maskData,
        *scaledMaskData, *intensiveData, *intensiveDataOriginal, *planeData;
    mglData *pWaveData, *nWaveData, *tWaveData;
    double pitchDataDerivativeZero;
    int waveDataLen;
    MetricsData metrics;
};

#endif // DRAWER_H
