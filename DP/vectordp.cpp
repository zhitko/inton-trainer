#include "vectordp.h"

#include "stdlib.h"

double VectorDP::getValueAt(vector array, int index)
{
    return array.v[index];
}

int VectorDP::getArraySize(vector array)
{
    return array.x;
}

void VectorDP::setValueAt(vector array, double value, int index)
{
    array.v[index] = value;
}

vector VectorDP::makeArray(int size)
{
    return makev(size);
}

void VectorDP::freeArray(vector array)
{
    freev(array);
}

int VectorDP::calculateError(double value1, double value2)
{
    return abs(value1 - value2);
}
