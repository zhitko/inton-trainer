#ifndef DP_H
#define DP_H

#include "defines.h"

#include <climits>
#include <math.h>
#include <limits>
#include <cstddef>

#include <QDebug>

#define DP_GLOBAL_LIMIT_ERROR 0.001

enum DPStateOperation{
    trHoriz = -1,
    trDiag = 0,
    trVert = 1,
    opNone = 2,
    opFirst = 3
};

struct DPState{
    double localError;
    double globalError;
    DPStateOperation operation;
    int signalPos;
    int patternPos;
    int time;
};

struct DPStateStack{
    DPState value;
    DPStateStack * next;
};

template< typename ValueType >
class Signal
{
public:
    virtual Signal<ValueType> * makeSignal(int size) = 0;

    virtual void freeSignal() = 0;
    virtual int size() = 0;
    virtual ValueType valueAt(int index) = 0;
    virtual void setValueAt(ValueType value, int index) = 0;
};

template< typename ValueType >
class DP
{
protected:
    int globalLimit;
    double localLimit;
    double a, b, c1, c2, d, aabb;
    DPStateStack * mask;
    double kH, kV, kD, kT, kQ;
    DPStateStack ** stateCache;
    Signal<ValueType> * pattern;
    Signal<ValueType> * signal;
    int signalSize;
    int patternSize;

    virtual DPStateStack * getStateCache(const int signalPos, const int patternPos)
    {
        if (stateCache == 0) this->reinitCache();
        return &(stateCache[signalPos][patternPos]);
    }

    virtual double getNormKt(const int signalPos, const int patternPos, int t)
    {
        return kT*abs(signalSize*patternPos - patternSize*signalPos)/kQ;
    }

    virtual DPStateStack * calcNextIter(const int signalPos, const int patternPos)
    {
//        qDebug() << "DPStateStack::calcNextIter " << signalPos << " " << patternPos << LOG_DATA;
        DPStateStack * currentBranch = this->getStateCache(signalPos, patternPos);
        // Check state cache for value
        if(currentBranch->value.operation != opNone)
            return currentBranch;

        static int iter = 0;
        ++iter;

        // Init current errors
        double localError = this->calculateError(
                    this->signal->valueAt(signalPos),
                    this->pattern->valueAt(patternPos)
                );

        DPState currentState = {localLimit * localError, localError, opFirst, signalPos, patternPos, 0};
        currentBranch->value = currentState;

        if (patternPos == 0) return currentBranch;

        DPStateStack * branchVert    = 0;
        DPStateStack * branchDiag    = 0;
        DPStateStack * branchHoriz   = 0;

        // Try next operations
        if(isGlobalPass(signalPos-1, patternPos-1) && patternPos > 0 && signalPos > 0) branchDiag  = calcNextIter(signalPos-1, patternPos-1);
        if(isGlobalPass(signalPos-1, patternPos  ) && signalPos  > 0)                  branchHoriz = calcNextIter(signalPos-1, patternPos);
        if(isGlobalPass(signalPos  , patternPos-1) && patternPos > 0)                  branchVert  = calcNextIter(signalPos,   patternPos-1);

        // If this is last iteration, return current branch
        if(branchHoriz == 0 && branchDiag == 0 && branchVert == 0)
            return currentBranch;

        // Init last states for all branches
        DPState stateDiag    = {localError, localError, trDiag,  signalPos-1, patternPos-1, 0};
        DPState stateVert = {localError, localError, trVert,  signalPos,   patternPos-1, 0};
        DPState stateHoriz   = {localError, localError, trHoriz, signalPos-1, patternPos, 0};

        double globalDiag = std::numeric_limits<double>::max();
        double globalVert = std::numeric_limits<double>::max();
        double globalHoriz = std::numeric_limits<double>::max();

        // Set value to state if branch calculated
        if(branchDiag != 0)
        {
            stateDiag = branchDiag->value;
            globalDiag = currentBranch->value.localError*kD
                    + stateDiag.globalError
                    + this->getNormKt(signalPos-1, patternPos-1, branchDiag->value.time+1);
        }
        if(branchVert != 0)
        {
            stateVert = branchVert->value;
            globalVert = currentBranch->value.localError*kV
                    + stateVert.globalError
                    + this->getNormKt(signalPos,   patternPos-1, branchVert->value.time);
        }
        if(branchHoriz != 0)
        {
            stateHoriz = branchHoriz->value;
            globalHoriz = currentBranch->value.localError*kH
                    + stateHoriz.globalError
                    + this->getNormKt(signalPos-1, patternPos, branchHoriz->value.time+1);
        }

        // Search branch with minimal global error
        if( globalDiag <= globalHoriz && globalDiag <= globalVert)
        { // Add operation
            currentBranch->next = branchDiag;
            currentBranch->value.globalError = globalDiag;
            currentBranch->value.operation = trDiag;
            if(patternPos != 0)
                currentBranch->value.time = branchDiag->value.time + 1;
        }
        else
        if( globalHoriz <= globalDiag && globalHoriz <= globalVert)
        { // Drop operation
            currentBranch->next = branchHoriz;
            currentBranch->value.globalError = globalHoriz;
            currentBranch->value.operation = trHoriz;
            if(patternPos != 0)
                currentBranch->value.time = branchHoriz->value.time + 1;
        }
        else
        if( globalVert <= globalDiag && globalVert <= globalHoriz)
        { // Repeat operation
            currentBranch->next = branchVert;
            currentBranch->value.globalError = globalVert;
            currentBranch->value.operation = trVert;
            if(patternPos != 0)
                currentBranch->value.time = branchVert->value.time;
        }

        return currentBranch;
    }

public:
    DP(Signal<ValueType> * pttrn, Signal<ValueType> * sig, int global, double local) :
        kH(0.1),
        kV(1.0),
        kD(0.5),
        kT(20),
        pattern(pttrn),
        signal(sig),
        mask(0),
        stateCache(0),
        globalLimit(global),
        localLimit(local),
        signalSize(sig->size()),
        patternSize(pttrn->size())
    {
        kQ = sqrt(patternSize*patternSize + signalSize*signalSize);
        double x11 = 0.0;
        double y11 = global;
        double x12 = signalSize - global;
        double y12 = patternSize;
        double x21 = global;
        double y21 = 0.0;
        double x22 = signalSize;
        double y22 = patternSize - global;
        a = y11 - y12;
        b = x12 - x11;
        c1 = x11 * y12 - x12 * y11;
        c2 = x21 * y22 - x22 * y21;
        aabb = sqrt(a*a+b*b);
        d = abs(c1-c2)/aabb;
    }

    ~DP()
    {
        pattern->freeSignal();
        signal->freeSignal();
        delete pattern;
        delete signal;
    }

    void applySettings(double kh, double kv, double kd, double kt)
    {
        qDebug() << this->kH << " to " << kh << LOG_DATA;
        this->kH = kh;
        qDebug() << this->kD << " to " << kd << LOG_DATA;
        this->kD = kd;
        qDebug() << this->kT << " to " << kt << LOG_DATA;
        this->kT = kt;
        qDebug() << this->kV << " to " << kv << LOG_DATA;
        this->kV = kv;
    }

    virtual bool isGlobalPass(int sX,int pY)
    {
        return true;
//        if ( (sX == 0 || pY == 0) && sX != pY ) return false;
//        double dd = abs( (a*sX+b*pY+c1)/aabb ) + abs( (a*sX+b*pY+c2)/aabb );
//        return globalLimit == -1 || (abs(d-dd) <= DP_GLOBAL_LIMIT_ERROR);
    }

    virtual int getSignalSize()
    {
        return this->signalSize;
    }

    virtual int getPatternSize()
    {
        return this->patternSize;
    }

    virtual DPStateStack * getSignalMask()
    {
        if(!this->mask)
        {
            qDebug() << "DP Init " << this->signalSize << " : " << this->patternSize << " iterations->" << this->signalSize*this->patternSize << LOG_DATA;
            this->mask = this->calcNextIter(this->signalSize - 1, this->patternSize - 1);
            qDebug() << "DP Finish " << this->stateCache[this->signalSize - 1][this->patternSize - 1].value.globalError << LOG_DATA;
        }
        return this->mask;
    }

    virtual Signal<ValueType> * getScaledSignal()
    {
        return this->applyMask(this->signal);
    }

    virtual void reinitCache()
    {
        if(stateCache)
        {
            for(int i=0; i<this->signalSize; i++)
                delete[] stateCache[i];
            delete[] stateCache;
        }

        DPState stubState = {0, 0, opNone};
        stateCache = new DPStateStack* [this->signalSize];
        for(int i=0; i<this->signalSize; i++)
        {
            stateCache[i] = new DPStateStack[this->patternSize];
            for(int j=0; j<this->patternSize; j++)
            {
                stateCache[i][j].value = stubState;
                stateCache[i][j].next = 0;
            }
        }
    }

    template< typename SignalValueType >
    Signal<SignalValueType> * applyMask(Signal<SignalValueType> * array)
    {
        if(!this->mask){
            reinitCache();
            getSignalMask();
        }
        if(signal->size() != array->size()){
            qDebug() << QString("Array has different size (%1 - %2)").arg(signal->size()).arg(array->size()) << LOG_DATA;
            return array;
        }

        Signal<SignalValueType> * result = array->makeSignal(this->patternSize);

        DPStateStack * stateStep = this->mask;
        DPStateOperation operation = opNone;
        while(stateStep != 0)
        {
            operation = stateStep->value.operation;
            switch (operation) {
                case trDiag:
                case trVert:
                    result->setValueAt(array->valueAt(stateStep->value.signalPos), stateStep->value.patternPos);
                    break;
                case trHoriz:
                    break;
                default:
                    break;
            }
            stateStep = stateStep->next;
        }

        return result;
    }

    int * getTemplateMapping()
    {
        if(!this->mask){
            reinitCache();
            getSignalMask();
        }

        int * templateIndex = new int[this->patternSize];
        for (int i=0; i<this->patternSize; i++) templateIndex[i] = i;

        int * templateMapping = new int[this->patternSize];
        for (int i=0; i<this->patternSize; i++) templateMapping[i] = 0;

        DPStateStack * stateStep = this->mask;
        DPStateOperation operation = opNone;
        while(stateStep != 0)
        {
            operation = stateStep->value.operation;
            switch (operation) {
                case trDiag:
                case trVert:
                    templateMapping[stateStep->value.patternPos] = templateIndex[stateStep->value.signalPos];
                    break;
                case trHoriz:
                    break;
                default:
                    break;
            }
            stateStep = stateStep->next;
        }

        delete templateIndex;

        return templateMapping;
    }

protected:
    virtual double calculateError(ValueType value1, ValueType value2) = 0;
};

#endif // DP_H
