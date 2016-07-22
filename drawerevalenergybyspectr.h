#ifndef DRAWEREVALENERGYBYSPECTR_H
#define DRAWEREVALENERGYBYSPECTR_H

#include "drawerevalenergy.h"

class DrawerEvalEnergyBySpectr : public DrawerEvalEnergy
{
public:
    DrawerEvalEnergyBySpectr();
    virtual ~DrawerEvalEnergyBySpectr();

    void Proc(QString fname);
};

#endif // DRAWEREVALENERGYBYSPECTR_H
