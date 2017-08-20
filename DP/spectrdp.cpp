#include "spectrdp.h"

#include "stdlib.h"
#include "math.h"

double* SpectrSignal::valueAt(int index)
{
    return &(array.v[index * this->spectrSize]);
}

int SpectrSignal::size()
{
    return array.x / this->spectrSize;
}

int SpectrSignal::sizeSpectr()
{
    return this->spectrSize;
}

void SpectrSignal::setValueAt(double* value, int index)
{
    for(int i=0; i<this->spectrSize; i++)
        setv(array, index * this->spectrSize + i, value[i]);
}

Signal<double*> * SpectrSignal::makeSignal(int size)
{
    return new SpectrSignal(makev(size * this->spectrSize), this->spectrSize);
}

void SpectrSignal::freeSignal()
{
    freev(this->array);
}

double SpectrDP::calculateError(double* value1, double* value2)
{
    double result = 0.0;
    double v = 0.0;
    for(int i=0; i<this->spectrSize; i++)
    {
        v = value1[i] - value2[i];
        result += v*v;
    }
    return sqrt(result);
}


