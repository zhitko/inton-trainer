#ifndef DP_H
#define DP_H

#include <climits>
#include <math.h>

#include <QDebug>

#define DP_GLOBAL_LIMIT_ERROR 0.001

enum DPStateOperation{
    opDrop = -1,
    opAdd = 0,
    opRepeat = 1,
    opNone = 2,
    opFirst = 3
};

struct DPState{
    int localError;
    int globalError;
    DPStateOperation operation;
    int signalPos;
    int patternPos;
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
private:
    Signal<ValueType> * pattern;
    Signal<ValueType> * signal;
    DPStateStack * mask;
    DPStateStack ** stateCache;

    int globalLimit;
    double localLimit;
    double a, b, c1, c2, d, aabb;

    DPStateStack * calcNextIter(const int signalPos, const int patternPos)
    {
        // Check state cache for value
        if(stateCache[signalPos][patternPos].value.operation != opNone)
            return &(stateCache[signalPos][patternPos]);

        static int iter = 0;
        ++iter;

        // Init current errors
        int localError = this->calculateError(
                    this->signal->valueAt(signalPos),
                    this->pattern->valueAt(patternPos)
                );

        DPState currentState = {localLimit * localError, 0, opNone, signalPos, patternPos};
        DPStateStack * currentBranch = &(stateCache[signalPos][patternPos]);
        currentBranch->value = currentState;

        DPStateStack * branchRepeat = 0;
        DPStateStack * branchAdd    = 0;
        DPStateStack * branchDrop   = 0;

        // Try next operations
        if(isGlobalPass(signalPos-1, patternPos-1) && patternPos > 0 && signalPos > 0) branchAdd    = calcNextIter(signalPos-1, patternPos-1);
        if(isGlobalPass(signalPos-1, patternPos  ) && signalPos  > 0)                  branchDrop   = calcNextIter(signalPos-1, patternPos);
        if(isGlobalPass(signalPos  , patternPos-1) && patternPos > 0)                  branchRepeat = calcNextIter(signalPos,   patternPos-1);

        // If this is last iteration, return current branch
        if(branchDrop == 0 && branchAdd == 0 && branchRepeat == 0)
        {
            currentBranch->value.operation = opFirst;
            return currentBranch;
        }

        // Init last states for all branches
        DPState stateAdd    = {INT_MAX, INT_MAX, opAdd,    signalPos-1, patternPos-1};
        DPState stateRepeat = {INT_MAX, INT_MAX, opRepeat, signalPos,   patternPos-1};
        DPState stateDrop   = {INT_MAX, INT_MAX, opDrop,   signalPos-1, patternPos};

        // Set value to state if branch calculated
        if(branchAdd    != 0) stateAdd    = branchAdd->value;
        if(branchRepeat != 0) stateRepeat = branchRepeat->value;
        if(branchDrop   != 0) stateDrop   = branchDrop->value;

        // Search branch with minimal global error
        if( stateAdd.globalError <= stateDrop.globalError &&
            stateAdd.globalError <= stateRepeat.globalError)
        { // Add operation
//            qDebug() << "Add";
            currentBranch->next = branchAdd;
            currentBranch->value.globalError = localLimit * currentBranch->value.localError + stateAdd.globalError;
            currentBranch->value.operation = opAdd;
        }
        else
        if( stateRepeat.globalError <= stateAdd.globalError &&
            stateRepeat.globalError <= stateDrop.globalError)
        { // Repeat operation
//            qDebug() << "Repeat";
            currentBranch->next = branchRepeat;
            currentBranch->value.globalError = currentBranch->value.localError + stateRepeat.globalError;
            currentBranch->value.operation = opRepeat;
        }
        else
        if( stateDrop.globalError <= stateAdd.globalError &&
            stateDrop.globalError <= stateRepeat.globalError)
        { // Drop operation
//            qDebug() << "Drop";
            currentBranch->next = branchDrop;
            currentBranch->value.globalError = currentBranch->value.localError + stateDrop.globalError;
            currentBranch->value.operation = opDrop;
        }

        return currentBranch;
    }

public:
    DP(Signal<ValueType> * pttrn, Signal<ValueType> * sig, int global, double local) :
        pattern(pttrn),
        signal(sig),
        mask(0),
        stateCache(0),
        globalLimit(global),
        localLimit(local)
    {
        double x11 = 0.0;
        double y11 = global;
        double x12 = sig->size() - global;
        double y12 = pttrn->size();
        double x21 = global;
        double y21 = 0.0;
        double x22 = sig->size();
        double y22 = pttrn->size() - global;
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

    bool isGlobalPass(int sX,int pY)
    {
        if ( (sX == 0 || pY == 0) && sX != pY ) return false;
        double dd = abs( (a*sX+b*pY+c1)/aabb ) + abs( (a*sX+b*pY+c2)/aabb );
        return globalLimit == -1 || (abs(d-dd) <= DP_GLOBAL_LIMIT_ERROR);
    }

    int getSignalSize()
    {
        return this->signal->size();
    }

    int getPatternSize()
    {
        return this->pattern->size();
    }

    DPStateStack * getSignalMask()
    {
        if(!this->mask)
        {
            int signalSize = this->signal->size();
            int patternSize = this->pattern->size();
            qDebug() << "DP Init " << signalSize << " : " << patternSize << " iterations->" << signalSize*patternSize;
            this->mask = this->calcNextIter(signalSize - 1, patternSize - 1);
            qDebug() << "DP Finish " << this->stateCache[signalSize - 1][patternSize - 1].value.globalError;
        }
        return this->mask;
    }

    Signal<ValueType> * getScaledSignal()
    {
        return this->applyMask(this->signal);
    }

    void reinitCache()
    {
        int signalSize = this->signal->size();
        int resultSize = this->pattern->size();

        if(stateCache)
        {
            for(int i=0; i<signalSize; i++)
                delete[] stateCache[i];
            delete[] stateCache;
        }

        DPState stabState = {INT_MAX, INT_MAX, opNone};
        stateCache = new DPStateStack* [signalSize];
        for(int i=0; i<signalSize; i++)
        {
            stateCache[i] = new DPStateStack[resultSize];
            for(int j=0; j<resultSize; j++)
            {
                stateCache[i][j].value = stabState;
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
            qDebug() << QString("Array has different size (%1 - %2)").arg(signal->size()).arg(array->size());
            return array;
        }

        int resultSize = pattern->size();

        Signal<SignalValueType> * result = array->makeSignal(resultSize);

        DPStateStack * stateStep = this->mask;

//        qDebug() << "opDrop = -1";
//        qDebug() << "opAdd = 0";
//        qDebug() << "opRepeat = 1";
//        qDebug() << "opNone = 2";
//        qDebug() << "opFirst = 3";
//        qDebug() << "operation\t global\t local\t signal\t pattern\t next";
        DPStateOperation operation = opNone;
        while(stateStep != 0)
        {
            operation = stateStep->value.operation;
//            qDebug() << stateStep->value.operation << " \t "
//                     << stateStep->value.globalError << " \t "
//                     << stateStep->value.localError << " \t "
//                     << stateStep->value.signalPos << " \t "
//                     << stateStep->value.patternPos << " \t "
//                     << stateStep->next;
            switch (operation) {
                case opAdd:
                case opRepeat:
                    result->setValueAt(array->valueAt(stateStep->value.signalPos), stateStep->value.patternPos);
                    break;
                case opDrop:
                    break;
                default:
//                    qDebug() << "opNone";
                    break;
            }
            stateStep = stateStep->next;
        }

        return result;
    }

protected:
    virtual int calculateError(ValueType value1, ValueType value2) = 0;
};

#endif // DP_H
