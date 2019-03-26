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

double SpectrDP::calculateError(int value1Pos, int value2Pos)
{
    double* value1 = this->signal->valueAt(value1Pos);
    double* value2 = this->pattern->valueAt(value2Pos);

    double result = 0.0;
    for(int i=0; i<this->spectrSize; i++)
    {
        double y = 0.0;
        double x = 0.0;
        if (isfinite(value1[i]))
        {
            x = value1[i];
        }
        if (isfinite(value2[i]))
        {
            y = value2[i];
        }
//        qDebug() << "SpectrDP::calculateError " << x << " " << y << LOG_DATA;
        result += pow(x - y, 2.0);
    }
    if (result > 0)
    {
        return sqrt(result);
    } else {
        return result;
    }
}


