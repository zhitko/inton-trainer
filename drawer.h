#ifndef DRAWER_H
#define DRAWER_H

#include "defines.h"

#include <mgl2/qmathgl.h>

#define GRAPH_Y_VAL_MAX 10000

#include "DP/vectordp.h"
#include "DP/spectrdp.h"
#include "dataprocessing.h"

extern "C" {
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

    virtual int getDataLenght();
    virtual int getDataSeconds();

    virtual int Draw(mglGraph *gr);
    virtual void Proc(QString fname);
    double calcResultMark(vector, vector);

    void Reload();
    void Click();
protected:
    GraphData * data;
    QString fileName;
    mglData *waveData, *pitchData, *pitchDataOriginal, *specData, *maskData, *scaledMaskData, *intensiveData, *intensiveDataOriginal;
    mglData *pWaveData, *nWaveData, *tWaveData;
    int waveDataLen;
};

#endif // DRAWER_H
