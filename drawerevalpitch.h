#ifndef DRAWEREVALPITCH_H
#define DRAWEREVALPITCH_H

#include "drawer.h"

class DrawerEvalPitch : public Drawer
{
public:
    DrawerEvalPitch();
    virtual ~DrawerEvalPitch();

    int Draw(mglGraph *gr);
    void Proc(QString fname);
    double result;

protected:
    bool first;
    QString secFileName;
    mglData secWaveData, secPitchData, secPitchDataOrig;
    mglData pSecWaveData, nSecWaveData, tSecWaveData;
};

#endif // DRAWEREVALPITCH_H
