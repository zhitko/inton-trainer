#ifndef CONTINUOUSDP_H
#define CONTINUOUSDP_H

#include "defines.h"

#include "spectrdp.h"

class ContinuousDP : public SpectrDP
{
public:
    ContinuousDP(SpectrSignal * pttrn, SpectrSignal * sig, int dpSignalScale, int errorLimit);
    virtual void addNewSignal(SpectrSignal * sig);
    virtual bool calculate();
    virtual void reinitCache();
    virtual vector getErrorVector();
    virtual vector getTimeVector();
    virtual intvector getMapping(int);
protected:
    int signalShift;
    int signalOffset;
    int cacheSize;
    int limit;
    double minimum;
    virtual DPStateStack * getStateCache(const int signalPos, const int patternPos);
    virtual double getNormKt(const int signalPos, const int patternPos, int t);
    virtual DPStateStack * calcNextIter(const int signalPos, const int patternPos);
};

#endif // CONTINUOUSDP_H
