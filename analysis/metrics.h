#ifndef METRICS_H
#define METRICS_H

#include "./sptk/vector.h"

#define METRIC_PROXIMITY_CURVE_CORRELATION                   0
#define METRIC_PROXIMITY_CURVE_INTEGRAL                      1
#define METRIC_PROXIMITY_CURVE_LOCAL                         2
#define METRIC_PROXIMITY_AVERAGE                             3
#define METRIC_PROXIMITY_RANGE                               4
#define METRIC_PROXIMITY_CURVE_SHAPE                         5
#define METRIC_TEMPLATE_F0_MIN                               6
#define METRIC_TEMPLATE_F0_MAX                               7
#define METRIC_RECORD_F0_MIN                                 8
#define METRIC_RECORD_F0_MAX                                 9
#define METRIC_TEMPO_RECORDED                                10
#define METRIC_TEMPO_TEMPLATE                                11
#define METRIC_RELATIVE_TEMPO                                12
#define METRIC_RMS_VOLUME_RECORDED                           13
#define METRIC_RMS_VOLUME_TEMPLATE                           14
#define METRIC_RELATEVE_RMS_VOLUME                           15
#define METRIC_REGISTER_F0_RECORDED                          16
#define METRIC_REGISTER_F0_TEMPLATE                          17
#define METRIC_RELATIVE_REGISTER_F0                          18
#define METRIC_DIAPASON_F0_RECORDED                          19
#define METRIC_DIAPASON_F0_TEMPLATE                          20
#define METRIC_RELATIVE_DIAPASON_F0                          21
#define METRIC_MEAN_VALUE_UMP_RECORDED                       22
#define METRIC_MEAN_VALUE_UMP_TEMPLATE                       23
#define METRIC_RELATIVE_MEAN_UMP                             24
#define METRIC_RMS_UMP_RECORDED                              25
#define METRIC_RMS_UMP_TEMPLATE                              26
#define METRIC_RELATIVE_RMS_UMP                              27
#define METRIC_CENTER_GRAVITY_UMP_RECORDED                   28
#define METRIC_CENTER_GRAVITY_UMP_RECORDED_1                 29
#define METRIC_CENTER_GRAVITY_UMP_RECORDED_2                 30
#define METRIC_CENTER_GRAVITY_UMP_RECORDED_MID               31
#define METRIC_CENTER_GRAVITY_UMP_RECORDED_LENGHT            32
#define METRIC_CENTER_GRAVITY_UMP_TEMPLATE                   33
#define METRIC_CENTER_GRAVITY_UMP_TEMPLATE_1                 34
#define METRIC_CENTER_GRAVITY_UMP_TEMPLATE_2                 35
#define METRIC_CENTER_GRAVITY_UMP_TEMPLATE_MID               36
#define METRIC_CENTER_GRAVITY_UMP_TEMPLATE_LENGHT            37
#define METRIC_RELATIVE_CENTER_GRAVITY_UMP                   38
#define METRIC_RELATIVE_CENTER_GRAVITY_UMP_1                 39
#define METRIC_RELATIVE_CENTER_GRAVITY_UMP_2                 40
#define METRIC_RELATIVE_CENTER_GRAVITY_UMP_LENGHT            41
#define METRIC_MEAN_VOLUME_RECORDED                          42
#define METRIC_MEAN_VOLUME_TEMPLATE                          43
#define METRIC_RELATEVE_MEAN_VOLUME                          44
#define METRIC_TEMPLATE_DIAPASON_F0                          45
#define METRIC_TEMPLATE_REGISTER_F0                          46
#define METRIC_MEAN_VALUE_UMP_DERIVATIVE_RECORDED            47
#define METRIC_MEAN_VALUE_UMP_DERIVATIVE_TEMPLATE            48
#define METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_RECORDED        49
#define METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_RECORDED_1      50
#define METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_RECORDED_2      51
#define METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_RECORDED_MID    52
#define METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_RECORDED_LENGHT 53
#define METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_TEMPLATE        54
#define METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_TEMPLATE_1      55
#define METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_TEMPLATE_2      56
#define METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_TEMPLATE_MID    57
#define METRIC_CENTER_GRAVITY_UMP_DERIVATIVE_TEMPLATE_LENGHT 58

#define METRIC_DATA_SIZE 59

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
