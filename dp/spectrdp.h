#ifndef SPECTRDP_H
#define SPECTRDP_H

#include "defines.h"

#include "dp.h"

extern "C" {
    #include "sptk/vector.h"
}

class SpectrSignal : public Signal<double*>
{
private:
    vector array;
    int spectrSize;
public:
    SpectrSignal(vector array, int dataSize) : array(array), spectrSize(dataSize) {;}
    Signal<double*> * makeSignal(int size);
    void freeSignal();

    int size();
    int sizeSpectr();
    double* valueAt(int index);
    void setValueAt(double* value, int index);

    vector getArray(){ return array; }
};

class SpectrDP : public DP<double*>
{
public:
    SpectrDP(SpectrSignal * pttrn, SpectrSignal * sig, int global, double local) : DP(pttrn, sig, global, local), spectrSize(pttrn->sizeSpectr()) {;}

    SpectrSignal * getScaledSignal() { return (SpectrSignal*) DP<double*>::getScaledSignal(); }

protected:
    int spectrSize;
    virtual double calculateError(int value1Pos, int value2Pos);
};

#endif // SPECTRDP_H
