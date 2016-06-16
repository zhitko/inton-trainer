#ifndef DRAWEREVALPITCHVIASPECTR_H
#define DRAWEREVALPITCHVIASPECTR_H

#include "drawer.h"

class DrawerEvalPitchViaSpectr : public Drawer
{
public:
    DrawerEvalPitchViaSpectr();
    ~DrawerEvalPitchViaSpectr();

    int Draw(mglGraph *gr);
    void Proc(QString fname);
    int result;

private:
    bool first;
    QString secFileName;
    mglData secWaveData, secPitchData, secPitchDataOrig;
};

#endif // DRAWEREVALPITCHVIASPECTR_H
