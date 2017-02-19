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
    mglData *dpData, *secWaveData, *errorData, *timeData, *secPitchData, *secIntensiveData;
    mglData *pSecData, *nSecData, *tSecData;
    int errorMax, errorMin;
    double result;

    SimpleGraphData * simple_data;
};

#endif // DRAWERDP_H
