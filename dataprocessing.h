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
    vector d_full_wave;
    vector d_pitch_original;
    vector d_pitch;                     int b_pitch;
    vector d_pitch_norm;                int b_pitch_norm;
    vector d_pitch_log;                 int b_pitch_log;
    vector d_pitch_derivative;          int b_pitch_derivative;
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
    vector d_mask;

    MaskData md_p;
    MaskData md_t;
    MaskData md_n;

    WaveFile * file_data;
};

vector data_get_pitch(SimpleGraphData * data);
vector data_get_pitch_norm(SimpleGraphData * data);
vector data_get_pitch_log(SimpleGraphData * data);
vector data_get_pitch_derivative(SimpleGraphData * data);
vector data_get_intensive(SimpleGraphData * data);
vector data_get_intensive_cutted(SimpleGraphData * data);
vector data_get_intensive_norm(SimpleGraphData * data);
vector data_get_intensive_derivative(SimpleGraphData * data);
vector data_get_intensive_smooth(SimpleGraphData * data);
vector data_spectrum(SimpleGraphData * data);
vector data_spectrum_norm(SimpleGraphData * data);
vector data_cepstrum(SimpleGraphData * data);
vector data_cepstrum_norm(SimpleGraphData * data);

struct GraphData: SimpleGraphData{
    vector d_wave;
    double pitch_min;
    double pitch_max;
    vector d_p_wave;
    vector d_n_wave;
    vector d_t_wave;
    vector p_mask;
    vector n_mask;
    vector t_mask;
    vector pnt_mask;
};

GraphData * ProcWave2Data(QString fname);
SimpleGraphData * SimpleProcWave2Data(QString fname, bool keepWaveData = false);
void freeGraphData(GraphData * data);
void freeSimpleGraphData(SimpleGraphData * data);

MaskData getLabelsFromFile(WaveFile* waveFile, char marker);

#endif // DATAPROCESSING_H
