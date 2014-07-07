#ifndef DRAWEREVALENERGY_H
#define DRAWEREVALENERGY_H

#include "drawer.h"

class DrawerEvalEnergy : public Drawer
{
public:
    DrawerEvalEnergy();
    ~DrawerEvalEnergy();

    int Draw(mglGraph *gr);
    void Proc(QString fname);
    int result;

private:
    bool first;
    QString secFileName;
    mglData secWaveData, secIntensiveData, secIntensiveDataOrig;
};

#endif // DRAWEREVALENERGY_H
