#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include <QString>

extern "C" {
    #include "./SPTK/vector.h"
    #include "./OpenAL/wavFile.h"
}

struct MaskData{
    intvector pointsFrom;
    intvector pointsLength;
};

struct SimpleGraphData{
    vector d_full_wave;
    vector d_pitch_originl;
    vector d_pitch;
    vector d_intensive_original;
    vector d_intensive;
    vector d_spec_proc;
    vector d_spec;

    MaskData md_p;
    MaskData md_t;
    MaskData md_n;
};

struct GraphData: SimpleGraphData{
//    vector d_full_wave;
    vector d_wave;
//    vector d_pitch_originl;
//    vector d_pitch;
    double pitch_min;
    double pitch_max;
//    vector d_intensive_original;
//    vector d_intensive;
    vector d_avg_intensive;
//    vector d_spec;
//    vector d_spec_proc;
    vector d_mask;
    vector d_p_wave;
    vector d_n_wave;
    vector d_t_wave;
    vector p_mask;
    vector n_mask;
    vector t_mask;
    vector pnt_mask;

//    MaskData md_p;
//    MaskData md_n;
//    MaskData md_t;
};

GraphData * ProcWave2Data(QString fname);
SimpleGraphData * SimpleProcWave2Data(QString fname);
void freeGraphData(GraphData * data);
void freeSimpleGraphData(SimpleGraphData * data);

MaskData getLabelsFromFile(WaveFile* waveFile, char marker);

#endif // DATAPROCESSING_H
