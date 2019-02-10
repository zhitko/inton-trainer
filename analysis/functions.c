#include "functions.h"

double norm(double value, double min, double max, double scale)
{
    return value / (max - min) * scale;
}

double norm1(double value, double max)
{
    return norm(value, 0.0, max, 1.0);
}

double norm100(double value, double max)
{
    return norm(value, 0.0, max, 100.0);
}
