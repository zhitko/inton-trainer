#ifndef DRAWER_H
#define DRAWER_H

#include <mgl2/qmathgl.h>

#define GRAPH_Y_VAL_MAX 10000

extern "C" {
    #include "./SPTK/SPTK.h"
    #include "./SPTK/vector.h"
}

struct GraphData{
    vector d_wave;
    vector d_pitch;
    vector d_log;
    vector d_intensive;
    vector d_avg_intensive;
    vector d_frame;
    vector d_window;
    vector d_lpc;
    vector d_spec;
};

void vectorToData(vector vec, mglData * data);
GraphData ProcWave2Data(QString fname);
void freeGraphData(GraphData data);

class Drawer: public mglDraw
{
public:
    Drawer();
    ~Drawer();

    double waveMin, waveMax,
           pitchMin, pitchMax,
           specMin, specMax;
    bool stereo;

    void specAuto();
    void pitchAuto();

    int getDataLenght();

    virtual int Draw(mglGraph *gr);
    virtual void Proc(QString fname);

    void Reload();
    void Click();
protected:
    GraphData * data;
    QString fileName;
    mglData waveData, pitchData, specData, intensiveData, logData, midIntensiveData;
    double _waveMin, _waveMax, _pitchMin, _pitchMax;
    int waveDataLen;
};

#endif // DRAWER_H
