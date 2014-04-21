#ifndef DRAWER_H
#define DRAWER_H

#include <mgl2/qmathgl.h>

class Drawer: public mglDraw
{
public:
    Drawer(QString fname);
    ~Drawer();

    double waveMin, waveMax, pitchMin, pitchMax, specMin, specMax;
    bool stereo;

    void specAuto();
    void pitchAuto();

    int getDataLenght();

    int Draw(mglGraph *gr);
    void Reload();
    void Click();
protected:
    QString fileName;
    mglData waveData, pitchData, specData;
    double _waveMin, _waveMax, _pitchMin, _pitchMax;
    int waveDataLen;
};

#endif // DRAWER_H
