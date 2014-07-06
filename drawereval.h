#ifndef DRAWEREVAL_H
#define DRAWEREVAL_H

#include "drawer.h"

class DrawerEval : public Drawer
{
public:
    DrawerEval();
    ~DrawerEval();

    int Draw(mglGraph *gr);
    void Proc(QString fname);
    int result;

private:
    bool first;
    QString secFileName;
    mglData secWaveData, secPitchData, secPitchDataOrig;
};

#endif // DRAWEREVAL_H
