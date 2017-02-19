#include "dataprocessing.h"
#include <QFile>
#include <QDebug>

#include "settingsdialog.h"

extern "C" {
    #include "./SPTK/SPTK.h"
    #include "./SPTK/pitch/pitch.h"
    #include "./SPTK/x2x/x2x.h"
    #include "./SPTK/frame/frame.h"
    #include "./SPTK/window/window.h"
    #include "./SPTK/lpc/lpc.h"
    #include "./SPTK/spec/spec.h"
    #include "./others/func.h"
    #include "./others/interpolation.h"
}

MaskData getLabelsFromFile(WaveFile* waveFile, char marker)
{
    MaskData data;// = new MaskData();
    data.pointsFrom.x = 0;
    data.pointsLength.x = 0;
    data.pointsFrom.v = NULL;
    data.pointsLength.v = NULL;

    qDebug() << "Use file data for mask";

    if (!((waveFile->cueChunk != NULL)
            && (littleEndianBytesToUInt16(waveFile->cueChunk->cuePointsCount) > 0)
            && (waveFile->listChunk != NULL)
            && (waveFile->listChunk->ltxtChunks != NULL)
            && (waveFile->listChunk->ltxtCount > 0)
            && (waveFile->listChunk->lablChunks != NULL)
            && (waveFile->listChunk->lablCount > 0)))
            return data;

    int cuePointsCount = littleEndianBytesToUInt16(waveFile->cueChunk->cuePointsCount);

    int *pointsFrom = (int*) malloc(sizeof(int)*cuePointsCount);
    int *pointsLength = (int*) malloc(sizeof(int)*cuePointsCount);


    qDebug() << "cueChunks";
    for(int i=0; i<cuePointsCount; i++)
    {
        CuePoint point = waveFile->cueChunk->cuePoints[i];
        int id = littleEndianBytesToUInt16(point.cuePointID);
        int pos = littleEndianBytesToUInt16(point.frameOffset);
        pointsFrom[id-1]=pos;
        pointsLength[id-1]=0;
        qDebug() << "cueChunk cuePointID " << id << " frameOffset " << pos;
    }

    qDebug() << "ltxtCount";
    for(int i=0; i<waveFile->listChunk->ltxtCount; i++)
    {
        LtxtChunk ltxt = waveFile->listChunk->ltxtChunks[i];
        int id = littleEndianBytesToUInt16(ltxt.cuePointID);
        int length = littleEndianBytesToUInt32(ltxt.sampleLength);
        pointsLength[id-1] = length;
        qDebug() << "ltxtChunk cuePointID " << id << " sampleLength " << length;
    }

    qDebug() << "lablChunks";
    if (marker != NULL)
    {
        int *markedPointsFrom = NULL;
        int *markedPointsLength = NULL;
        int count = 0;
        for(int i=0; i<waveFile->listChunk->lablCount; i++)
        {
            LablChunk labl = waveFile->listChunk->lablChunks[i];
            int id = littleEndianBytesToUInt16(labl.cuePointID);
            char * text = waveFile->listChunk->lablChunks[i].text;
            if (text[0] == marker)
            {
                count++;
                markedPointsFrom = (int*) realloc(markedPointsFrom, sizeof(int)*count);
                markedPointsLength = (int*) realloc(markedPointsLength, sizeof(int)*count);
                markedPointsFrom[count-1] = pointsFrom[id-1];
                markedPointsLength[count-1] = pointsLength[id-1];
            }
            qDebug() << "lablChunk cuePointID " << id << " text " << text[0];
        }
        free(pointsFrom);
        free(pointsLength);
        pointsFrom = markedPointsFrom;
        pointsLength = markedPointsLength;
        cuePointsCount = count;
    }

    data.pointsFrom.x = cuePointsCount;
    data.pointsLength.x = cuePointsCount;
    data.pointsFrom.v = pointsFrom;
    data.pointsLength.v = pointsLength;

    return data;
}

vector readMaskFromFile(WaveFile* waveFile, int length, char marker)
{
    vector mask_from_file;

    MaskData data = getLabelsFromFile(waveFile, marker);

    mask_from_file = make_mask(length, data.pointsFrom.x, data.pointsFrom.v, data.pointsLength.v);
    qDebug() << "make_mask";

    freeiv(data.pointsFrom);
    freeiv(data.pointsLength);

    return mask_from_file;
}

bool validateMask(vector mask)
{
    bool valid = false;
    for (int i=0; i<mask.x; i++)
    {
        if (mask.v[i] == 1)
        {
            valid = true;
        }
    }
    return valid;
}

vector getFileMask(WaveFile* waveFile, vector wave, int len, char marker = NULL)
{
    vector mask;
    bool tryFileData = (waveFile->cueChunk != NULL)
            && (littleEndianBytesToUInt16(waveFile->cueChunk->cuePointsCount) > 0)
            && (waveFile->listChunk != NULL)
            && (waveFile->listChunk->ltxtChunks != NULL)
            && (waveFile->listChunk->ltxtCount > 0)
            && (waveFile->listChunk->lablChunks != NULL)
            && (waveFile->listChunk->lablCount > 0);

//    qDebug() << "waveFile->cueChunk " << (waveFile->cueChunk != NULL);
//    qDebug() << "waveFile->listChunk " << (waveFile->listChunk != NULL);
//    qDebug() << "waveFile->listChunk->ltxtCount " << (waveFile->listChunk->ltxtCount);
//    qDebug() << "waveFile->listChunk->lablCount " << (waveFile->listChunk->lablCount);

    if (tryFileData)
    {
        qDebug() << "tryFileData";
        vector mask_from_file = readMaskFromFile(waveFile, wave.x, marker);
        mask = vector_resize(mask_from_file, len);
        freev(mask_from_file);
        qDebug() << "vector_resize";
    }

    if (!tryFileData || !validateMask(mask)) {
        qDebug() << "!tryFileData";
        mask = onesv(len);
    }

    return mask;
}

vector processZeros(vector data)
{
    bool needProcess = false;
    int start = 0;
    int end = 0;
    for (int i=0; i<data.x; i++)
    {
        end = i - 1;
        if (data.v[i] != 0 && needProcess)
        {
            needProcess = false;
            for (int j=end; j>=start; j--)
            {
                data.v[j] = data.v[i];
            }
        }
        else if (data.v[i] == 0 && !needProcess)
        {
            start = i;
            needProcess = true;
        }
    }
    if (needProcess)
    {
        end++;
        for (int i=start; i<=end; i++)
        {
            data.v[i] = data.v[start-1];
        }
    }
    return data;
}

GraphData * ProcWave2Data(QString fname)
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    QFile file(fname);
    file.open(QIODevice::ReadOnly);
    WaveFile * waveFile = waveOpenHFile(file.handle());
    qDebug() << "waveOpenFile";

    int size = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
    qDebug() << "chunkDataSize";
    short int bits = littleEndianBytesToUInt16(waveFile->formatChunk->significantBitsPerSample);
    qDebug() << "significantBitsPerSample";

    vector wave = sptk_v2v(waveFile->dataChunk->waveformData, size, bits);
    qDebug() << "wave";

    vector frame = sptk_frame(wave, sptk_settings->frame);
    qDebug() << "frame";

    vector intensive = vector_intensive(frame, sptk_settings->frame);
    qDebug() << "intensive";

    vector intensive_avg = vector_avg_intensive(intensive, sptk_settings->energyFrame);
    qDebug() << "intensive_avg";

    vector window = sptk_window(frame, sptk_settings->window);
    qDebug() << "window";

    vector lpc = sptk_lpc(frame, sptk_settings->lpc);
    qDebug() << "lpc";

    vector spec = sptk_spec(lpc, sptk_settings->spec);
    qDebug() << "spec " << maxv(spec);

    vector spec_proc;
    if (sptk_settings->spec->proc == 0){
        spec_proc = vector_pow_log(spec, sptk_settings->spec->factor, sptk_settings->spec->min);
        qDebug() << "spec_log";
    } else if (sptk_settings->spec->proc == 1){
        spec_proc = vector_pow_exp(spec, sptk_settings->spec->factor, sptk_settings->spec->min);
        qDebug() << "spec_exp";
    }
    qDebug() << "spec_proc " << maxv(spec_proc);

    vector pitch = processZeros(sptk_pitch_spec(wave, sptk_settings->pitch, intensive.x));
    qDebug() << "pitch";

    vector mask = getFileMask(waveFile, wave, pitch.x);
    qDebug() << "mask";

    vector pitch_cutted = processZeros(vector_cut_by_mask(pitch, mask));
    qDebug() << "pitch_cutted";
    double pitch_min = pitch_cutted.v[minv(pitch_cutted)];
    double pitch_max = pitch_cutted.v[maxv(pitch_cutted)];

    vector intensive_cutted = vector_cut_by_mask(intensive, mask);
    qDebug() << "intensive_cutted";

    vector inverted_mask = vector_invert_mask(mask);
    qDebug() << "inverted_mask";

    vector pitch_interpolate = vector_interpolate_by_mask(
                pitch_cutted,
                inverted_mask,
                sptk_settings->plotF0->interpolation_edges,
                sptk_settings->plotF0->interpolation_type
                );
    qDebug() << "pitch_interpolate";

    vector intensive_interpolate = vector_interpolate_by_mask(
                intensive_cutted,
                inverted_mask,
                sptk_settings->plotEnergy->interpolation_edges,
                sptk_settings->plotEnergy->interpolation_type
                );
    qDebug() << "intensive_interpolate";

    vector pitch_mid = vector_mid(pitch_interpolate, sptk_settings->plotF0->midFrame);
    qDebug() << "pitch_mid";

    vector intensive_mid = vector_mid(intensive_interpolate, sptk_settings->plotEnergy->midFrame);
    qDebug() << "intensive_mid";

    vector norm_wave = normalizev(wave, 0.0, 1.0);

    MaskData md_p = getLabelsFromFile(waveFile, 'P');
    MaskData md_n = getLabelsFromFile(waveFile, 'N');
    MaskData md_t = getLabelsFromFile(waveFile, 'T');

    vector p_mask = readMaskFromFile(waveFile, wave.x, 'P');
    qDebug() << "p_mask";

    vector n_mask = readMaskFromFile(waveFile, wave.x, 'N');
    qDebug() << "n_mask";

    vector t_mask = readMaskFromFile(waveFile, wave.x, 'T');
    qDebug() << "t_mask";

    vector p_wave = zero_to_nan(vector_cut_by_mask(norm_wave, p_mask));
    qDebug() << "p_mask";

    vector n_wave = zero_to_nan(vector_cut_by_mask(norm_wave, n_mask));
    qDebug() << "n_mask";

    vector t_wave = zero_to_nan(vector_cut_by_mask(norm_wave, t_mask));
    qDebug() << "t_mask";

    vector pnt_mask = onesv(norm_wave.x);
    for (int i=0; i<p_mask.x && i<n_mask.x && i<t_mask.x && i<norm_wave.x; i++) {
        if (p_mask.v[i] == 1 || n_mask.v[i] == 1 || t_mask.v[i] == 1)
        {
            pnt_mask.v[i] = 0;
        } else {
            pnt_mask.v[i] = 1;
        }
    }

    vector display_wave = zero_to_nan(vector_cut_by_mask(norm_wave, pnt_mask));

    freev(frame);
    freev(window);
    freev(lpc);
    freev(pitch_cutted);
    freev(intensive_cutted);
    freev(inverted_mask);
    freev(pitch_interpolate);
    freev(intensive_interpolate);
    freev(wave);
    qDebug() << "freev";

    file.close();
    waveCloseFile(waveFile);
    qDebug() << "waveCloseFile";

    GraphData * data = new GraphData();

    data->d_full_wave = norm_wave;
    data->d_wave = display_wave;
    data->d_p_wave = p_wave;
    data->d_n_wave = n_wave;
    data->d_t_wave = t_wave;
    data->d_pitch_originl = pitch;
    data->d_pitch = pitch_mid;
    data->pitch_max = pitch_max;
    data->pitch_min = pitch_min;
    data->d_intensive_original = intensive;
    data->d_intensive = intensive_mid;
    data->d_avg_intensive = intensive_avg;
    data->d_spec_proc = spec_proc;
    data->d_spec = spec;
    data->d_mask = mask;
    data->p_mask = p_mask;
    data->n_mask = n_mask;
    data->t_mask = t_mask;
    data->pnt_mask = pnt_mask;

    data->md_p = md_p;
    data->md_t = md_t;
    data->md_n = md_n;

    return data;
}

SimpleGraphData * SimpleProcWave2Data(QString fname)
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    QFile file(fname);
    file.open(QIODevice::ReadOnly);
    WaveFile * waveFile = waveOpenHFile(file.handle());
    qDebug() << "waveOpenFile";

    int size = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
    qDebug() << "chunkDataSize";
    short int bits = littleEndianBytesToUInt16(waveFile->formatChunk->significantBitsPerSample);
    qDebug() << "significantBitsPerSample";

    vector wave = sptk_v2v(waveFile->dataChunk->waveformData, size, bits);
    qDebug() << "wave";

    vector norm_wave = normalizev(wave, 0.0, 1.0);
    qDebug() << "norm_wave";

    vector frame = sptk_frame(wave, sptk_settings->frame);
    qDebug() << "frame";

    vector intensive = vector_intensive(frame, sptk_settings->frame);
    qDebug() << "intensive";

    vector window = sptk_window(frame, sptk_settings->window);
    qDebug() << "window";

    vector lpc = sptk_lpc(frame, sptk_settings->lpc);
    qDebug() << "lpc";

    vector spec = sptk_spec(lpc, sptk_settings->spec);
    qDebug() << "spec " << maxv(spec);

    vector spec_proc;
    if (sptk_settings->spec->proc == 0){
        spec_proc = vector_pow_log(spec, sptk_settings->spec->factor, sptk_settings->spec->min);
        qDebug() << "spec_log";
    } else if (sptk_settings->spec->proc == 1){
        spec_proc = vector_pow_exp(spec, sptk_settings->spec->factor, sptk_settings->spec->min);
        qDebug() << "spec_exp";
    }
    qDebug() << "spec_proc " << maxv(spec_proc);

    vector pitch = processZeros(sptk_pitch_spec(wave, sptk_settings->pitch, intensive.x));
    qDebug() << "pitch";

    vector mask = getFileMask(waveFile, wave, pitch.x);
    qDebug() << "mask";

    vector inverted_mask = vector_invert_mask(mask);
    qDebug() << "inverted_mask";

    vector pitch_interpolate = vector_interpolate_by_mask(
                pitch,
                inverted_mask,
                sptk_settings->plotF0->interpolation_edges,
                sptk_settings->plotF0->interpolation_type
                );
    qDebug() << "pitch_interpolate";

    vector pitch_mid = vector_mid(pitch_interpolate, sptk_settings->plotF0->midFrame);
    qDebug() << "pitch_mid";

    vector intensive_mid = vector_mid(intensive, sptk_settings->plotEnergy->midFrame);
    qDebug() << "intensive_mid";

    MaskData md_p = getLabelsFromFile(waveFile, 'P');
    MaskData md_n = getLabelsFromFile(waveFile, 'N');
    MaskData md_t = getLabelsFromFile(waveFile, 'T');

    freev(frame);
    freev(window);
    freev(lpc);
    freev(wave);
    freev(pitch_interpolate);
    freev(inverted_mask);
    qDebug() << "freev";

    file.close();
    waveCloseFile(waveFile);
    qDebug() << "waveCloseFile";

    SimpleGraphData * data = new SimpleGraphData();

    data->d_full_wave = norm_wave;
    data->d_pitch_originl = pitch;
    data->d_pitch = pitch_mid;
    data->d_intensive_original = intensive;
    data->d_intensive = intensive_mid;
    data->d_spec_proc = spec_proc;
    data->d_spec = spec;
    data->d_mask = mask;

    data->md_p = md_p;
    data->md_t = md_t;
    data->md_n = md_n;

    return data;
}

void freeGraphData(GraphData * data)
{
    freev(data->d_intensive_original);
    freev(data->d_intensive);
    freev(data->d_avg_intensive);
    freev(data->d_pitch_originl);
    freev(data->d_pitch);
    freev(data->d_spec);
    freev(data->d_spec_proc);
    freev(data->d_wave);
    freev(data->d_full_wave);
    freev(data->d_mask);
    freev(data->d_p_wave);
    freev(data->d_t_wave);
    freev(data->d_n_wave);
    freev(data->p_mask);
    freev(data->n_mask);
    freev(data->t_mask);
    freev(data->pnt_mask);
}

void freeSimpleGraphData(SimpleGraphData * data)
{
    freev(data->d_full_wave);
    freev(data->d_pitch_originl);
    freev(data->d_pitch);
    freev(data->d_intensive_original);
    freev(data->d_intensive);
    freev(data->d_spec_proc);
    freev(data->d_spec);

    freeiv(data->md_p.pointsFrom);
    freeiv(data->md_p.pointsLength);
    freeiv(data->md_t.pointsFrom);
    freeiv(data->md_t.pointsLength);
    freeiv(data->md_n.pointsFrom);
    freeiv(data->md_n.pointsLength);
}