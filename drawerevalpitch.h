#ifndef DRAWEREVALPITCH_H
#define DRAWEREVALPITCH_H

#include "drawer.h"

class DrawerEvalPitch : public Drawer
{
public:
    DrawerEvalPitch();
    ~DrawerEvalPitch();

    int Draw(mglGraph *gr);
    void Proc(QString fname);
    int result;

private:
    bool first;
    QString secFileName;
    mglData secWaveData, secPitchData, secPitchDataOrig;
};

#endif // DRAWEREVALPITCH_H
