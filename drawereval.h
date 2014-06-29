#ifndef DRAWEREVAL_H
#define DRAWEREVAL_H

#include "drawer.h"

class DrawerEval : public Drawer
{
public:
    DrawerEval(QString fname);
    ~DrawerEval();

    int Draw(mglGraph *gr);
    void Proc(QString fname);

private:
    QString secFileName;
    mglData secWaveData, secPitchData, secIntensiveData, secPitchDataOrig, secIntensiveDataOrig;
    double _secWaveMin, _secWaveMax, _secPitchMin, _secPitchMax;
    int secWaveDataLen;
    vector dsec_wave, dsec_pitch, dsec_frame, dsec_intensive, dsec_window;
};

#endif // DRAWEREVAL_H
