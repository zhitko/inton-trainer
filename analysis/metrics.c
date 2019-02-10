#include "metrics.h"
#include "math.h"


MetricsData createMetricData()
{
    MetricsData data;

    for (int i=0; i < METRIC_DATA_SIZE; i++)
    {
        data.calculated[i] = 0;
        data.values[i] = NAN;
    }

    return data;
}

MetricsData storeMetric(MetricsData metrics, int index, double value)
{
    if (index < 0 || index >= METRIC_DATA_SIZE)
    {
        return metrics;
    }

    metrics.calculated[index] = 1;
    metrics.values[index] = value;

    return metrics;
}

MetricsData generateRelativeMetric(MetricsData metrics, int index, int index1, int index2)
{
    metrics = storeMetric(
        metrics,
        index,
        10 * log10(getMetric(metrics, index1) / getMetric(metrics, index2))
    );
    return metrics;
}

double getMetric(MetricsData metrics, int index)
{
    if (index < 0 || index >= METRIC_DATA_SIZE)
    {
        return -1.0;
    }

    return metrics.values[index];
}

int hasMetric(MetricsData metrics, int index)
{
    if (index < 0 || index >= METRIC_DATA_SIZE)
    {
        return 0;
    } else if (isnan(metrics.values[index]))
    {
        return 0;
    } else {
        return 1;
    }
}


double calculateRMS(vector data, double mean_value)
{
    double result = 0.0;
    for (int i=0; i<data.x; i++)
    {
        result += pow(getv(data, i) - mean_value, 2);
    }
    result = sqrt(result);
    return result;
}

double calculateCentricGravitySubvector(vector data, int from, int to)
{
    double result = 0.0;

    if (from < 0) from = 0;
    if (to > data.x) to = data.x;

    double px = 0.0;
    double p = 0.0;
    for (int i=from; i<to; i++)
    {
        px += getv(data, i) * i;
        p += getv(data, i);
    }
    result = px / p;

    return result;
}

double calculateCentricGravity(vector data)
{
    return calculateCentricGravitySubvector(data, 0, data.x);
}
