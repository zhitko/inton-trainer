#ifndef DRAWERDP_H
#define DRAWERDP_H

#include "defines.h"

#include "drawer.h"

class DrawerDP : public Drawer
{
public:
    DrawerDP();
    virtual ~DrawerDP();

    virtual int Draw(mglGraph *gr);
    void Proc(QString fname);
    virtual int getDataSeconds();

private:
    QString getMarksTitle();

protected:
    bool first;
    QString secFileName;
    mglData *dpData, *secWaveData, *errorData, *timeData, *secPitchData, *secPitchDataDerivative,
        *secIntensiveData, *umpData, *secUmpData, *umpDerivativeData, *secUmpDerivativeData, *umpMask,
        *octavData, *secOctavData, *secPlaneData;
    mglData *pSecData, *nSecData, *tSecData;
    int errorMax, errorMin;
    double proximity_shape_mark, proximity_range_mark;
    double secPitchDataDerivativeZero;
    int f0min, f0max;
    int userf0min, userf0max;
    double rt, ru;
    double meanValueUMP, userMeanValueUMP;
    double centricGravityUMP, userCentricGravityUMP;

    SimpleGraphData * simple_data;
};

#endif // DRAWERDP_H
