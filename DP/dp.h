#ifndef DP_H
#define DP_H

#include <climits>

#include <QDebug>

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

template< typename A, typename V >
class DP
{
private:
    A pattern;
    A signal;
    DPStateStack * mask;
    DPStateStack ** stateCache;

    DPStateStack * calcNextIter(const int signalPos, const int patternPos)
    {
        // Check state cache for value
        if(stateCache[signalPos][patternPos].value.operation != opNone)
            return &(stateCache[signalPos][patternPos]);

        static int iter = 0;
        ++iter;

        // Init current errors
        int localError = this->calculateError(
                    this->getValueAt(this->signal, signalPos),
                    this->getValueAt(this->pattern, patternPos)
                );

        DPState currentState = {localError, localError, opNone, signalPos, patternPos};
        DPStateStack * currentBranch = &(stateCache[signalPos][patternPos]);
        currentBranch->value = currentState;

        DPStateStack * branchRepeat = 0;
        DPStateStack * branchAdd    = 0;
        DPStateStack * branchDrop   = 0;

        // Try next operations
        if(signalPos  > 0)                  branchDrop   = calcNextIter(signalPos-1, patternPos);
        if(patternPos > 0 && signalPos > 0) branchAdd    = calcNextIter(signalPos-1, patternPos-1);
        if(patternPos > 0)                  branchRepeat = calcNextIter(signalPos,   patternPos-1);

        // If this is last iteration, return current branch
        if(branchDrop == 0 && branchAdd == 0 && branchRepeat == 0)
        {
            currentBranch->value.operation = opFirst;
            return currentBranch;
        }

        // Init last states for all branches
        DPState stateRepeat = {INT_MAX, INT_MAX, opRepeat, signalPos,   patternPos-1};
        DPState stateAdd    = {INT_MAX, INT_MAX, opAdd,    signalPos-1, patternPos-1};
        DPState stateDrop   = {INT_MAX, INT_MAX, opDrop,   signalPos-1, patternPos};

        // Set value to state if branch calculated
        if(branchRepeat != 0) stateRepeat = branchRepeat->value;
        if(branchAdd    != 0) stateAdd    = branchAdd->value;
        if(branchDrop   != 0) stateDrop   = branchDrop->value;

        // Search branch with minimal global error
        if( stateAdd.globalError <= stateDrop.globalError &&
            stateAdd.globalError <= stateRepeat.globalError)
        { // Add operation
            currentBranch->next = branchAdd;
            currentBranch->value.globalError = currentBranch->value.localError + stateAdd.globalError;
            currentBranch->value.operation = opAdd;
        }
        else
        if( stateRepeat.globalError <= stateAdd.globalError &&
            stateRepeat.globalError <= stateDrop.globalError)
        { // Repeat operation
            currentBranch->next = branchRepeat;
            currentBranch->value.globalError = currentBranch->value.localError + stateRepeat.globalError;
            currentBranch->value.operation = opRepeat;
        }
        else
        if( stateDrop.globalError <= stateAdd.globalError &&
            stateDrop.globalError <= stateRepeat.globalError)
        { // Drop operation
            currentBranch->next = branchDrop;
            currentBranch->value.globalError = currentBranch->value.localError + stateDrop.globalError;
            currentBranch->value.operation = opDrop;
        }

        return currentBranch;
    }

public:
    DP(A pttrn, A sig) :
        pattern(pttrn),
        signal(sig),
        mask(0),
        stateCache(0)
    { ; }

    DPStateStack * getSignalMask()
    {
        if(!this->mask)
        {
            int signalSize = this->getArraySize(signal);
            int patternSize = this->getArraySize(pattern);
            qDebug() << "DP Init " << signalSize << " : " << patternSize << " iterations->" << signalSize*patternSize;
            this->mask = this->calcNextIter(signalSize - 1, patternSize - 1);
            qDebug() << "DP Finish " << this->stateCache[signalSize - 1][patternSize - 1].value.globalError;
        }
        return this->mask;
    }

    A getScaledSignal()
    {
        return this->applyMask(this->signal);
    }

    void reinitCache()
    {
        int signalSize = getArraySize(signal);
        int resultSize = getArraySize(pattern);

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

    A applyMask(A array)
    {
        if(!this->mask){
            reinitCache();
            getSignalMask();
        }
        if(getArraySize(signal) != getArraySize(array)) return array;

        int resultSize = getArraySize(pattern);

        A result = makeArray(resultSize);

        DPStateStack * stateStep = this->mask;

        qDebug() << "opDrop = -1";
        qDebug() << "opAdd = 0";
        qDebug() << "opRepeat = 1";
        qDebug() << "opNone = 2";
        qDebug() << "opFirst = 3";
        qDebug() << "operation\t global\t local\t signal\t pattern\t next";
        DPStateOperation operation = opNone;
        while(stateStep != 0)
//        while(stateStep->next != 0)
        {
//            stateStep = stateStep->next;
            operation = stateStep->value.operation;
            qDebug() << stateStep->value.operation << " \t "
                     << stateStep->value.globalError << " \t "
                     << stateStep->value.localError << " \t "
                     << stateStep->value.signalPos << " \t "
                     << stateStep->value.patternPos << " \t "
                     << stateStep->next;
            switch (operation) {
            case opAdd:
            case opRepeat:
                setValueAt(result, getValueAt(array, stateStep->value.signalPos), stateStep->value.patternPos);
                break;
            case opDrop:
                break;
            default:
                qDebug() << "opNone";
                break;
            }
            stateStep = stateStep->next;
        }

        return result;
    }

protected:
    virtual V getValueAt(A array, int index) = 0;
    virtual int getArraySize(A array) = 0;
    virtual void setValueAt(A array, V value, int index) = 0;
    virtual A makeArray(int size) = 0;
    virtual void freeArray(A array) = 0;
    virtual int calculateError(V value1, V value2) = 0;
};

#endif // DP_H
