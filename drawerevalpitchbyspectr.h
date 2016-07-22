#ifndef DRAWEREVALPITCHBYSPECTR_H
#define DRAWEREVALPITCHBYSPECTR_H

#include "drawerevalpitch.h"

class DrawerEvalPitchBySpectr : public DrawerEvalPitch
{
public:
    DrawerEvalPitchBySpectr();
    virtual ~DrawerEvalPitchBySpectr();

    void Proc(QString fname);
};

#endif // DRAWEREVALPITCHBYSPECTR_H
