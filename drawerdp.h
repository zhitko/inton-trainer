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
    mglData secWaveData, errorData, timeData;
    mglData * dpData;
    int errorMax, errorMin;
};

#endif // DRAWERDP_H
