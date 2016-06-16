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

void SpectrSignal::setValueAt(double* value, int index)
{
    for(int i=0; i<this->spectrSize; i++)
        array.v[index * this->spectrSize + i] = value[i];
}

Signal<double*> * SpectrSignal::makeSignal(int size)
{
    return new SpectrSignal(makev(size * this->spectrSize), this->spectrSize);
}

void SpectrSignal::freeSignal()
{
    freev(this->array);
}

int SpectrDP::calculateError(double* value1, double* value2)
{
    int error = 0;
    for(int i=0; i<this->spectrSize; i++)
        error += (value1[i] - value2[i])*(value1[i] - value2[i]);
    return sqrt(error);
}
