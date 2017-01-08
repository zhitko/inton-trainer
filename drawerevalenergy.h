#ifndef DRAWEREVALENERGY_H
#define DRAWEREVALENERGY_H

#include "drawer.h"

class DrawerEvalEnergy : public Drawer
{
public:
    DrawerEvalEnergy();
    virtual ~DrawerEvalEnergy();

    virtual int Draw(mglGraph *gr);
    void Proc(QString fname);
    double result;

protected:
    bool first;
    QString secFileName;
    mglData *secWaveData, *secIntensiveData, *secIntensiveDataOrig;
};

#endif // DRAWEREVALENERGY_H
