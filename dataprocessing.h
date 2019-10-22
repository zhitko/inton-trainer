#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include "defines.h"

#include <QString>

extern "C" {
    #include "./sptk/vector.h"
    #include "openal/wavFile.h"
    #include "./analysis/mask_data.h"
}

struct SimpleGraphData{
    double seconds;
    vector d_full_wave;                 int b_full_wave;
    vector d_pitch_original;
    vector d_pitch;                     int b_pitch;
    vector d_pitch_log;                 int b_pitch_log;
    vector d_pitch_cutted;              int b_pitch_cutted;
    double d_pitch_min;                 int b_pitch_min;
    double d_pitch_max;                 int b_pitch_max;
    vector d_pitch_interpolated;        int b_pitch_interpolated;
    vector d_pitch_norm;                int b_pitch_norm;
    vector d_pitch_smooth;              int b_pitch_smooth;
    vector d_pitch_derivative;          int b_pitch_derivative;
    vector d_ump;                       int b_ump;
    MaskData d_ump_mask;
    vector d_intensive_original;
    vector d_intensive;                 int b_intensive;
    vector d_intensive_cutted;          int b_intensive_cutted;
    vector d_intensive_norm;            int b_intensive_norm;
    vector d_intensive_smooth;          int b_intensive_smooth;
    vector d_derivative_intensive_norm; int b_derivative_intensive_norm;
    vector d_spec_proc;
    vector d_spec;                      int b_spec;
    vector d_spec_norm;                 int b_spec_norm;
    vector d_cepstrum;                  int b_cepstrum;
    vector d_cepstrum_norm;             int b_cepstrum_norm;
    vector d_mask;                      int b_mask;

    MaskData d_file_p;                  int b_file_p;
    MaskData d_file_t;                  int b_file_t;
    MaskData d_file_n;                  int b_file_n;

    WaveFile * data_file;
};

vector data_get_full_wave(SimpleGraphData * data);
vector data_get_pitch(SimpleGraphData * data);
vector data_get_pitch_log(SimpleGraphData * data);
vector data_get_pitch_cutted(SimpleGraphData * data);
double data_get_pitch_min(SimpleGraphData * data);
double data_get_pitch_max(SimpleGraphData * data);
vector data_get_pitch_interpolated(SimpleGraphData * data);
vector data_get_pitch_norm(SimpleGraphData * data);
vector data_get_pitch_smooth(SimpleGraphData * data);
vector data_get_pitch_derivative(SimpleGraphData * data);
vector data_get_ump(SimpleGraphData * data, bool manual);
MaskData data_get_ump_mask(SimpleGraphData * data, bool manual);
vector data_get_intensive(SimpleGraphData * data);
vector data_get_intensive_cutted(SimpleGraphData * data);
vector data_get_intensive_norm(SimpleGraphData * data);
vector data_get_intensive_derivative(SimpleGraphData * data);
vector data_get_intensive_smooth(SimpleGraphData * data);
vector data_spectrum(SimpleGraphData * data);
vector data_spectrum_norm(SimpleGraphData * data);
vector data_cepstrum(SimpleGraphData * data);
vector data_cepstrum_norm(SimpleGraphData * data);
vector data_get_mask(SimpleGraphData * data);
MaskData data_get_p(SimpleGraphData * data);
MaskData data_get_t(SimpleGraphData * data);
MaskData data_get_n(SimpleGraphData * data);
WaveFile* data_get_data_file(SimpleGraphData * data);

SimpleGraphData * SimpleProcWave2Data(QString fname, bool manual);
void freeSimpleGraphData(SimpleGraphData * data);

MaskData getLabelsFromFile(WaveFile* waveFile, char marker);

#endif // DATAPROCESSING_H
