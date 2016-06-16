#ifndef SPECTRDP_H
#define SPECTRDP_H

#include "dp.h"

extern "C" {
    #include "vector.h"
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
    double* valueAt(int index);
    void setValueAt(double* value, int index);

    vector getArray(){ return array; }
};

class SpectrDP : public DP<double*>
{
public:
    SpectrDP(SpectrSignal * pttrn, SpectrSignal * sig) : DP(pttrn, sig), spectrSize(pttrn->size()) {;}

    SpectrSignal * getScaledSignal() { return (SpectrSignal*) DP<double*>::getScaledSignal(); }

protected:
    int spectrSize;
    int calculateError(double* value1, double* value2);
};

#endif // SPECTRDP_H
