#ifndef DRAWER_H
#define DRAWER_H

#include <mgl2/qmathgl.h>

class Drawer: public mglDraw
{
private:
    QString fileName;
    mglData waveData, pitchData, specData;
public:
    Drawer(QString fname);
    ~Drawer();

    double waveMin, waveMax, pitchMin, pitchMax, specMin, specMax;
    bool stereo;

    void specAuto();
    void pitchAuto();

    int Draw(mglGraph *gr);
    void Reload();
    void Click();
};

#endif // DRAWER_H
