#include "curve_similarity.h"

#include "utils.h"
#include "defines.h"

#include <cmath>

double calculateCurvesSimilarityCorrelation(vector x, vector y)
{
    double result = 0;
    double my = midv(y);
    double mx = midv(x);
    double xx = 0.0;
    double yy = 0.0;
    for(int i=0; i<x.x && i<y.x; i++)
    {
        result += (getv(x, i)-mx)*(getv(y, i)-my);
        xx += (getv(x, i)-mx)*(getv(x, i)-mx);
        yy += (getv(y, i)-my)*(getv(y, i)-my);
    }
    result = result / sqrt(xx*yy);
    return round(fabs((result+1.0)/2.0)*100);
}

double calculateCurvesSimilarityAverageDistance(vector x, vector y)
{
    double result = 0;
    for(int i=0; i<x.x && i<y.x; i++)
    {
        result += (getv(x, i)-getv(y, i))*(getv(x, i)-getv(y, i));
    }
    result = sqrt(result) / sqrt(x.x);
    return round((1-result)*100);
}

double calculateCurvesSimilarityMaxLocalDistance(vector x, vector y)
{
    vector subVector = subtractionv(x, y);
    vector absVector = absv(subVector);
    double result = getv(absVector, maxv(absVector));
    freev(subVector);
    freev(absVector);
    return round((1-result)*100);
}

double calculateCurvesSimilarityRelativeDistance(vector x, vector y)
{
    vector subVector = subtractionv(x, y);
    vector absVector = absv(subVector);
    double index = maxv(absVector);
    double vx = getv(x, index);
    double vy = getv(y, index);
    double result = round( 100.0 * MIN(vx, vy) / MAX(vx, vy) );
    freev(subVector);
    freev(absVector);
    return result;
}

double calculateCurvesSimilarityRelativeAverageDistance(vector x, vector y)
{
    double result = 0.0;
    double vx, vy;
    for(int i=0; i<MIN(x.x, y.x); i++)
    {
        vx = getv(x, i);
        vy = getv(y, i);
        result += MIN(vx, vy) / MAX(vx, vy);
    }
    result = round( 100.0 * result / MIN(x.x, y.x) );
    return result;
}
