#ifndef VECTORDP_H
#define VECTORDP_H

#include "dp.h"

extern "C" {
    #include "vector.h"
}

class VectorSignal : public Signal<double>
{
private:
    vector array;
public:
    VectorSignal(vector array) : array(array) {;}
    Signal<double> * makeSignal(int size);
    void freeSignal();

    int size();
    double valueAt(int index);
    void setValueAt(double value, int index);

    vector getArray(){ return array; }
};

class VectorDP : public DP<double>
{
public:
    VectorDP(VectorSignal * pttrn, VectorSignal * sig) : DP(pttrn, sig) {;}

    VectorSignal * getScaledSignal() { return (VectorSignal*) DP<double>::getScaledSignal(); }

protected:
    int calculateError(double value1, double value2);
};

#endif // VECTORDP_H
