#include "continuousdp.h"

#include <QDebug>

ContinuousDP::ContinuousDP(SpectrSignal * pttrn, SpectrSignal * sig, int dpSignalScale, int errorLimit):
    SpectrDP(pttrn, sig, -1, 1),
    signalShift(0),
    signalOffset(0),
    limit(errorLimit),
    minimum(std::numeric_limits<double>::max())
{
    this->signalShift = sig->size();
    this->cacheSize = this->signalShift*dpSignalScale;

    this->reinitCache();
}

bool ContinuousDP::calculate()
{
//    qDebug() << "ContinuousDP::calculate " << this->signalSize << " " << this->patternSize;
    this->calcNextIter(this->signalSize - 1, this->patternSize - 1);
    return false;
}

void ContinuousDP::addNewSignal(SpectrSignal *sig)
{
//    qDebug() << "addNewSignal";
    this->signal->freeSignal();
    delete this->signal;

    this->signal = sig;
    this->signalSize = this->signal->size();

    this->signalOffset += this->signalSize;

    /*
     *
     *  |-----------|-----------|-----------|
     *  1           5           10          15
     *                 ^ offset = 7
     *                             ^ offset + shift = 12
     *                 <-- clear -->
     *-----------------------------------------------
     *                             ^ offset = 12
     *                                         ^ offset + shift = 17
     *     ^ offset + shift - cachesize = 2
     *   -->                       <-- clear
     *
     *
     */

    if(this->signalOffset >= this->cacheSize)
    {
        this->signalOffset -= this->cacheSize;
    }

    qDebug() << this->minimum;
    this->minimum = std::numeric_limits<double>::max();

    DPState stubState = {0, 0, opNone};
    for(int i=0; i<this->signalSize; i++)
    {
        for(int j=0; j<this->patternSize; j++)
        {
            DPStateStack * cache = this->getStateCache(i, j);
            cache->value = stubState;
            cache->next = 0;
        }
    }
}

vector ContinuousDP::getErrorVector()
{
    vector errors = zerov(this->signalSize);
    for(int i=0; i<this->signalSize; i++)
    {
        errors.v[i] = this->getStateCache(i, this->patternSize - 1)->value.globalError;
    }
    return errors;
}

vector ContinuousDP::getTimeVector()
{
    vector time = zerov(this->signalSize);
    for(int i=0; i<this->signalSize; i++)
    {
        time.v[i] = this->getStateCache(i, this->patternSize - 1)->value.time;
    }
    return time;
}

intvector ContinuousDP::getMapping(int pos)
{
    intvector mapping = zeroiv(this->patternSize);
    int pattern_index = 1;
    int signal_index = 0;
    qDebug() << "signalSize " << this->signalSize;
    qDebug() << "patternSize " << this->patternSize;

    DPStateStack * stateStep = this->getStateCache(pos, this->patternSize - 1);
    DPStateOperation operation = opNone;
    while(stateStep != 0)
    {
        if (pattern_index < mapping.x)
        {
//            qDebug() << "mapping_index " << (mapping.x - pattern_index);
            mapping.v[mapping.x - pattern_index] = signal_index;
        } else {
            qDebug() << "WARNING getMapping " << pattern_index << " " << signal_index;
        }
        operation = stateStep->value.operation;
        switch (operation) {
            case trDiag:
                signal_index++;
                pattern_index++;
                break;
            case trVert:
                pattern_index++;
                break;
            case trHoriz:
                signal_index++;
                break;
            default:
                break;
        }
        stateStep = stateStep->next;
    }
    qDebug() << "Mask size pattern_index " << pattern_index << " (pattern size equal)";
    qDebug() << "Mask size signal_index " << signal_index << " (mached signal size equal)";

    for (int i=0; i<mapping.x; i++)
    {
        mapping.v[i] = signal_index - mapping.v[i];
    }

    return mapping;
}

double ContinuousDP::getNormKt(const int signalPos, const int patternPos, int t)
{
    return kT*abs(patternPos - t)/patternSize;
}

DPStateStack * ContinuousDP::calcNextIter(const int signalPos, const int patternPos)
{
//    qDebug() << "ContinuousDP::calcNextIter " << signalPos << " " << patternPos;
    DPStateStack * stack = SpectrDP::calcNextIter(signalPos, patternPos);
    if (patternPos == (this->patternSize - 1) && this->minimum > stack->value.globalError)
        this->minimum = stack->value.globalError;
    return stack;
}

DPStateStack * ContinuousDP::getStateCache(const int signalPos, const int patternPos)
{
//    qDebug() << "ContinuousDP::getStateCache " << signalPos << " " << patternPos;
    int pos = signalPos + this->signalOffset;
    if(pos >= this->cacheSize) pos -= this->cacheSize;
//    qDebug() << "ContinuousDP::getStateCache " << pos << " " << patternPos;
    return &(stateCache[pos][patternPos]);
}

void ContinuousDP::reinitCache()
{
    if(stateCache)
    {
        for(int i=0; i<this->cacheSize; i++)
            delete[] stateCache[i];
        delete[] stateCache;
    }

    DPState stubState = {0, 0, opNone};
    stateCache = new DPStateStack* [this->cacheSize];
    for(int i=0; i<this->cacheSize; i++)
    {
        stateCache[i] = new DPStateStack[this->patternSize];
        for(int j=0; j<this->patternSize; j++)
        {
            stateCache[i][j].value = stubState;
            stateCache[i][j].next = 0;
        }
    }
}
