#include "vectordp.h"

#include "stdlib.h"

double VectorSignal::valueAt(int index)
{
    return getv(array, index);
}

int VectorSignal::size()
{
    return array.x;
}

void VectorSignal::setValueAt(double value, int index)
{
    setv(array, index, value);
}

Signal<double> * VectorSignal::makeSignal(int size)
{
    return new VectorSignal(makev(size));
}

void VectorSignal::freeSignal()
{
    freev(this->array);
}

double VectorDP::calculateError(int value1Pos, int value2Pos)
{
    double value1 = this->signal->valueAt(value1Pos);
    double value2 = this->pattern->valueAt(value2Pos);
    return abs(value1 - value2);
}
