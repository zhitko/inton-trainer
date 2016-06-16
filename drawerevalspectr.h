#ifndef DRAWEREVALSPECTR_H
#define DRAWEREVALSPECTR_H

#include "drawer.h"

class DrawerEvalSpectr : public Drawer
{
public:
    DrawerEvalSpectr();
    ~DrawerEvalSpectr();

    int Draw(mglGraph *gr);
    void Proc(QString fname);
    int result;

private:
    bool first;
    QString secFileName;
    mglData secWaveData, secSpecData, secSpecDataOrig;
};

#endif // DRAWEREVALSPECTR_H
