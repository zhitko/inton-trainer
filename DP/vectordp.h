#ifndef VECTORDP_H
#define VECTORDP_H

#include "dp.h"

extern "C" {
    #include "vector.h"
}

class VectorDP : public DP<vector, double>
{
public:
    VectorDP(vector pttrn, vector sig) : DP(pttrn, sig) {;}

protected:
    double getValueAt(vector array, int index);
    int getArraySize(vector array);
    void setValueAt(vector array, double value, int index);
    vector makeArray(int size);
    void freeArray(vector array);
    int calculateError(double value1, double value2);
};

#endif // VECTORDP_H
