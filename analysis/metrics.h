#ifndef METRICS_H
#define METRICS_H

#include "./sptk/vector.h"

#define METRIC_PROXIMITY_CURVE_CORRELATION                   0
#define METRIC_PROXIMITY_CURVE_INTEGRAL                      1
#define METRIC_PROXIMITY_CURVE_LOCAL                         2
#define METRIC_PROXIMITY_AVERAGE                             3
#define METRIC_PROXIMITY_RANGE                               4
#define METRIC_PROXIMITY_CURVE_SHAPE                         5

#define METRIC_TEMPLATE_DIAPASON_F0                          6
#define METRIC_TEMPLATE_REGISTER_F0                          7

#define METRIC_DIAPASON_F0_RECORDED                          8
#define METRIC_RMS_VOLUME_RECORDED                           9
#define METRIC_TEMPO_RECORDED                                10
#define METRIC_RECORD_F0_MIN                                 11
#define METRIC_RECORD_F0_MAX                                 12
#define METRIC_REGISTER_F0_RECORDED                          13
#define METRIC_MEAN_VALUE_UMP_RECORDED                       14
#define METRIC_RMS_UMP_RECORDED                              15
#define METRIC_MEAN_VOLUME_RECORDED                          16
#define METRIC_CENTER_GRAVITY_UMP_RECORDED                   17
#define METRIC_CENTER_GRAVITY_UMP_RECORDED_1                 18
#define METRIC_CENTER_GRAVITY_UMP_RECORDED_2                 19
#define METRIC_CENTER_GRAVITY_UMP_RECORDED_MID               20
#define METRIC_CENTER_GRAVITY_UMP_RECORDED_LENGHT            21
#define METRIC_MEAN_VALUE_UMP_DERIVATIVE_RECORDED            22
#define METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_RECORDED        23
#define METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_RECORDED_1      24
#define METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_RECORDED_2      25
#define METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_RECORDED_MID    26
#define METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_RECORDED_LENGHT 27

#define METRIC_DIAPASON_F0_TEMPLATE                          28
#define METRIC_RMS_VOLUME_TEMPLATE                           29
#define METRIC_TEMPO_TEMPLATE                                30
#define METRIC_TEMPLATE_F0_MIN                               31
#define METRIC_TEMPLATE_F0_MAX                               32
#define METRIC_REGISTER_F0_TEMPLATE                          33
#define METRIC_MEAN_VALUE_UMP_TEMPLATE                       34
#define METRIC_RMS_UMP_TEMPLATE                              35
#define METRIC_MEAN_VOLUME_TEMPLATE                          36
#define METRIC_CENTER_GRAVITY_UMP_TEMPLATE                   37
#define METRIC_CENTER_GRAVITY_UMP_TEMPLATE_1                 38
#define METRIC_CENTER_GRAVITY_UMP_TEMPLATE_2                 39
#define METRIC_CENTER_GRAVITY_UMP_TEMPLATE_MID               40
#define METRIC_CENTER_GRAVITY_UMP_TEMPLATE_LENGHT            41
#define METRIC_MEAN_VALUE_UMP_DERIVATIVE_TEMPLATE            42
#define METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_TEMPLATE        43
#define METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_TEMPLATE_1      44
#define METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_TEMPLATE_2      45
#define METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_TEMPLATE_MID    46
#define METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_TEMPLATE_LENGHT 47

#define METRIC_RELATIVE_TEMPO                                48
#define METRIC_RELATEVE_RMS_VOLUME                           49
#define METRIC_RELATIVE_RMS_UMP                              50
#define METRIC_RELATIVE_MEAN_UMP                             51
#define METRIC_RELATIVE_CENTER_GRAVITY_UMP                   52
#define METRIC_RELATIVE_CENTER_GRAVITY_UMP_1                 53
#define METRIC_RELATIVE_CENTER_GRAVITY_UMP_2                 54
#define METRIC_RELATIVE_CENTER_GRAVITY_UMP_LENGHT            55
#define METRIC_RELATEVE_MEAN_VOLUME                          56
#define METRIC_RELATIVE_REGISTER_F0                          57
#define METRIC_RELATIVE_DIAPASON_F0                          58
#define METRIC_RELATIVE_CENTER_GRAVITY_UMP_DERIVATIVE        59
#define METRIC_RELATIVE_CENTER_GRAVITY_UMP_DERIVATIVE_1      60
#define METRIC_RELATIVE_CENTER_GRAVITY_UMP_DERIVATIVE_2      61
#define METRIC_RELATIVE_CENTER_GRAVITY_UMP_DERIVATIVE_LENGHT 62
#define METRIC_RELATIVE_F0_MAX                               63
#define METRIC_RELATIVE_F0_MIN                               64
#define METRIC_RELATIVE_CENTER_GRAVITY_UMP_MID               65
#define METRIC_RELATIVE_MEAN_VALUE_UMP_DERIVATIVE            66
#define METRIC_RELATIVE_CENTER_GRAVITY_UMP_DERIVATIVE_MID    67

#define METRIC_DATA_SIZE 68

typedef struct {
    double values[METRIC_DATA_SIZE];
    int calculated[METRIC_DATA_SIZE];
} MetricsData;

MetricsData createMetricData();

MetricsData storeMetric(MetricsData metrics, int index, double value);
MetricsData generateRelativeMetric(MetricsData metrics, int index, int index1, int index2);
double getMetric(MetricsData metrics, int index);
int hasMetric(MetricsData metrics, int index);

double calculateRMS(vector data, double mean_value);
double calculateCentricGravity(vector data);
double calculateCentricGravitySubvector(vector data, int from, int to);

#endif // METRICS_H
