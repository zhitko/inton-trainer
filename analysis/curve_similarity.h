#ifndef CURVE_SIMILARITY_H
#define CURVE_SIMILARITY_H

extern "C" {
    #include "sptk/vector.h"
}

/*
 * Correlation of F0-curves
*/
double calculateCurvesSimilarityCorrelation(vector, vector);

/*
 * Integral proximity of F0-curves
*/
double calculateCurvesSimilarityAverageDistance(vector, vector);

/*
 * Local proximity of F0-curves
*/
double calculateCurvesSimilarityMaxLocalDistance(vector, vector);

/*
 * Average proximity of F0-curves
*/
double calculateCurvesSimilarityRelativeDistance(vector, vector);

/*
 * Relative Average proximity of F0-curves
*/
double calculateCurvesSimilarityRelativeAverageDistance(vector, vector);

#endif // CURVE_SIMILARITY_H
