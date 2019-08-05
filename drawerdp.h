#ifndef DRAWERDP_H
#define DRAWERDP_H

#include "defines.h"

#include "drawer.h"
#include "dp/continuousdp.h"

class DrawerDP : public Drawer
{
public:
    DrawerDP();
    virtual ~DrawerDP();

    virtual int Draw(mglGraph *gr);
    void Proc(QString fname);
    virtual void reProc();
    virtual int getDataSeconds();

private:
    QString getMarksTitle();

protected:
    bool first;
    QString secFileName;
    QList<double> umpSectors;
    mglData *dpData, *secWaveData, *errorData, *timeData, *secPitchData, *secPitchDataDerivative,
        *secIntensiveData, *umpData, *secUmpData, *umpDerivativeData, *secUmpDerivativeData, *umpMask,
        *octavData, *secOctavData, *secPlaneData;
    mglData *pSecData, *nSecData, *tSecData;
    mglData *A0Smooth, *secA0Smooth;
    int errorMax, errorMin;
    double proximity_shape_mark, proximity_range_mark;
    double secPitchDataDerivativeZero;
    int f0min, f0max;
    int userf0min, userf0max;
    double range, userRange;
    double meanValueUMP, userMeanValueUMP;
    double meanDerivativeValueUMP, userDerivativeMeanValueUMP;
    double centricGravityUMP, userCentricGravityUMP;
    double centricGravityUMP1, userCentricGravityUMP1;
    double centricGravityUMP2, userCentricGravityUMP2;
    double centricGravityDerivativeUMP, userCentricGravityDerivativeUMP;
    double centricGravityDerivativeUMP1, userCentricGravityDerivativeUMP1;
    double centricGravityDerivativeUMP2, userCentricGravityDerivativeUMP2;

    SimpleGraphData * simple_data;

    ContinuousDP * getDP(SimpleGraphData * dataSec);
};

#endif // DRAWERDP_H
