#ifndef DRAWER_H
#define DRAWER_H

#include <mgl2/qmathgl.h>

extern "C" {
    #include "./SPTK/SPTK.h"
    #include "./SPTK/vector.h"
    vector sptk_intensive(vector data, FRAME_SETTINGS * settings);
}

class Drawer: public mglDraw
{
public:
    Drawer(QString fname);
    ~Drawer();

    double waveMin, waveMax,
           pitchMin, pitchMax,
           specMin, specMax;
    bool stereo;

    void specAuto();
    void pitchAuto();

    int getDataLenght();

    int Draw(mglGraph *gr);
    void Reload();
    void Click();
protected:
    QString fileName;
    mglData waveData, pitchData, specData, intensiveData;
    double _waveMin, _waveMax, _pitchMin, _pitchMax;
    int waveDataLen;
    vector d_wave, d_pitch, d_frame, d_intensive, d_window, d_lpc, d_spec;
};

#endif // DRAWER_H
