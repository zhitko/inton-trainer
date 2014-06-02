#ifndef ABSTRACTDRAWER_H
#define ABSTRACTDRAWER_H

#include <mgl2/qmathgl.h>

class IDrawer : public mglDraw
{
public:
    virtual ~IDrawer(){}

    virtual int Draw(mglGraph *)=0;
    virtual void Reload(){}
    virtual void Click(){}

    virtual int getDataLenght()=0;
    virtual void graphAuto(){}
};

#endif // ABSTRACTDRAWER_H
