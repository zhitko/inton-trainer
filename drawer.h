#ifndef DRAWER_H
#define DRAWER_H

#include <mgl2/qmathgl.h>

#define GRAPH_Y_VAL_MAX 10000

#include "DP/vectordp.h"
#include "DP/spectrdp.h"

extern "C" {
    #include "./SPTK/SPTK.h"
    #include "./SPTK/vector.h"
}

struct GraphData{
    vector d_full_wave;
    vector d_wave;
    vector d_pitch_originl;
    vector d_pitch;
    double pitch_min;
    double pitch_max;
    vector d_intensive_original;
    vector d_intensive;
    vector d_avg_intensive;
    vector d_spec;
    vector d_spec_proc;
    vector d_mask;
    vector d_p_wave;
    vector d_n_wave;
    vector d_t_wave;
    vector p_mask;
    vector n_mask;
    vector t_mask;
    vector pnt_mask;
};

void vectorToData(vector vec, mglData * data);
void vectorToDataWithNan(vector vec, mglData * data);
GraphData ProcWave2Data(QString fname);
void freeGraphData(GraphData data);
vector scaleVectorByDPResults(vector, SpectrDP*);
vector getSignalWithMask(vector, SpectrDP*, vector);

class Drawer: public mglDraw
{
public:
    Drawer();
    virtual ~Drawer();

    bool stereo;

    int getDataLenght();

    virtual int Draw(mglGraph *gr);
    virtual void Proc(QString fname);
    double calcResultMark(vector, vector, int);

    void Reload();
    void Click();
protected:
    GraphData * data;
    QString fileName;
    mglData waveData, pitchData, pitchDataOriginal, specData, maskData, scaledMaskData, intensiveData, intensiveDataOriginal, midIntensiveData;
    mglData pWaveData, nWaveData, tWaveData;
    int waveDataLen;
};

#endif // DRAWER_H
