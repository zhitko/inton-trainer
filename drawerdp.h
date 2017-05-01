#ifndef DRAWERDP_H
#define DRAWERDP_H

#include "drawer.h"

class DrawerDP : public Drawer
{
public:
    DrawerDP();
    virtual ~DrawerDP();

    virtual int Draw(mglGraph *gr);
    void Proc(QString fname);

protected:
    bool first;
    QString secFileName;
    mglData *dpData, *secWaveData, *errorData, *timeData, *secPitchData, *secIntensiveData, *umpData, *secUmpData, *umpMask, *octavData, *secOctavData;
    mglData *pSecData, *nSecData, *tSecData;
    int errorMax, errorMin;
    double proximity_shape, proximity_range, proximity_curve_shape;
    int f0min, f0max;
    int userf0min, userf0max;

    SimpleGraphData * simple_data;
};

#endif // DRAWERDP_H
