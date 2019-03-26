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
    #include "./SPTK/lpc2c/lpc2c.h"
    #include "./SPTK/spec/spec.h"
    #include "./others/func.h"
    #include "./others/interpolation.h"
}

#include "processing/filemarkout.h"

WaveFile * selectMarkoutAlgorithm(SimpleGraphData * data);

MaskData getLabelsFromFile(WaveFile* waveFile, char marker)
{
    MaskData data;
    data.pointsFrom.x = 0;
    data.pointsLength.x = 0;
    data.pointsFrom.v = NULL;
    data.pointsLength.v = NULL;

    qDebug() << "Use file data for mask" << LOG_DATA;

    ListChunk * listLablChunk = NULL;
    ListChunk * listLtxtChunk = NULL;

    for (int i=0; i<waveFile->listCount; i++)
    {
        ListChunk * listChunk = &(waveFile->listChunks[i]);
        if (listChunk == NULL) continue;

        qDebug() << "listChunk->lablCount " << listChunk->lablCount << LOG_DATA;
        if (listChunk->lablChunks != NULL && listChunk->lablCount > 0)
        {
            listLablChunk = listChunk;
        }

        qDebug() << "listChunk->ltxtCount " << listChunk->ltxtCount << LOG_DATA;
        if (listChunk->ltxtChunks != NULL && listChunk->ltxtCount > 0)
        {
            listLtxtChunk = listChunk;
        }
    }

    if (!((waveFile->cueChunk != NULL)
            && (littleEndianBytesToUInt16(waveFile->cueChunk->cuePointsCount) > 0)
            && (listLtxtChunk != NULL)
            && (listLtxtChunk->ltxtChunks != NULL)
            && (listLtxtChunk->ltxtCount > 0)
            && (listLablChunk != NULL)
            && (listLablChunk->lablChunks != NULL)
            && (listLablChunk->lablCount > 0)))
            return data;

    int cuePointsCount = littleEndianBytesToUInt16(waveFile->cueChunk->cuePointsCount);

    int *pointsFrom = (int*) malloc(sizeof(int)*cuePointsCount);
    int *pointsLength = (int*) malloc(sizeof(int)*cuePointsCount);
    int *pointsId = (int*) malloc(sizeof(int)*cuePointsCount);


    qDebug() << "cueChunks" << LOG_DATA;
    for(int i=0; i<cuePointsCount; i++)
    {
        CuePoint point = waveFile->cueChunk->cuePoints[i];
        int id = littleEndianBytesToUInt16(point.cuePointID);
        int pos = littleEndianBytesToUInt16(point.frameOffset);
        pointsFrom[i]=pos;
        pointsLength[i]=0;
        pointsId[i]=id;
        qDebug() << "cueChunk cuePointID " << id << " frameOffset " << pos << LOG_DATA;
    }

    qDebug() << "ltxtCount" << LOG_DATA;
    for(int i=0; i<listLtxtChunk->ltxtCount; i++)
    {
        LtxtChunk ltxt = listLtxtChunk->ltxtChunks[i];
        int id = littleEndianBytesToUInt16(ltxt.cuePointID);
        int length = littleEndianBytesToUInt32(ltxt.sampleLength);
        pointsLength[i] = length;
        qDebug() << "ltxtChunk cuePointID " << id << " sampleLength " << length << LOG_DATA;
    }

    qDebug() << "lablChunks" << LOG_DATA;
    if (marker != NULL)
    {
        int *markedPointsFrom = NULL;
        int *markedPointsLength = NULL;
        int count = 0;
        for(int i=0; i<listLablChunk->lablCount; i++)
        {
            LablChunk labl = listLablChunk->lablChunks[i];
            int id = littleEndianBytesToUInt16(labl.cuePointID);
            char * text = listLablChunk->lablChunks[i].text;
            if (toupper(text[0]) == toupper(marker))
            {
                count++;
                markedPointsFrom = (int*) realloc(markedPointsFrom, sizeof(int)*count);
                markedPointsLength = (int*) realloc(markedPointsLength, sizeof(int)*count);
                markedPointsFrom[count-1] = pointsFrom[i];
                markedPointsLength[count-1] = pointsLength[i];
            }
            qDebug() << "lablChunk cuePointID " << id << " text " << text[0] << LOG_DATA;
        }
        free(pointsFrom);
        free(pointsLength);
        pointsFrom = markedPointsFrom;
        pointsLength = markedPointsLength;
        cuePointsCount = count;
    }

    free(pointsId);

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
    qDebug() << "MaskData pointsFrom" << data.pointsFrom.x << LOG_DATA;
    qDebug() << "MaskData pointsLength" << data.pointsLength.x << LOG_DATA;

    mask_from_file = make_mask(length, data.pointsFrom.x, data.pointsFrom.v, data.pointsLength.v);
    qDebug() << "make_mask" << LOG_DATA;

    freeiv(data.pointsFrom);
    freeiv(data.pointsLength);

    return mask_from_file;
}

bool validateMask(vector mask)
{
    bool valid = false;
    for (int i=0; i<mask.x; i++)
    {
        if (getv(mask, i) == 1)
        {
            valid = true;
        }
    }
    return valid;
}

vector getFileMask(WaveFile* waveFile, vector wave, int len, char marker = NULL)
{
    vector mask;

    ListChunk * listLablChunk = NULL;
    ListChunk * listLtxtChunk = NULL;

    qDebug() << "listCount " << waveFile->listCount << LOG_DATA;

    for (int i=0; i<waveFile->listCount; i++)
    {
        ListChunk * listChunk = &(waveFile->listChunks[i]);
        if (listChunk == NULL) continue;

        qDebug() << "listChunk->lablCount " << listChunk->lablCount << LOG_DATA;
        if (listChunk->lablChunks != NULL && listChunk->lablCount > 0)
        {
            listLablChunk = listChunk;
        }

        qDebug() << "listChunk->ltxtCount " << listChunk->ltxtCount << LOG_DATA;
        if (listChunk->ltxtChunks != NULL && listChunk->ltxtCount > 0)
        {
            listLtxtChunk = listChunk;
        }
    }

    bool tryFileData = (waveFile->cueChunk != NULL)
            && (littleEndianBytesToUInt16(waveFile->cueChunk->cuePointsCount) > 0)
            && (listLtxtChunk != NULL)
            && (listLtxtChunk->ltxtChunks != NULL)
            && (listLtxtChunk->ltxtCount > 0)
            && (listLablChunk != NULL)
            && (listLablChunk->lablChunks != NULL)
            && (listLablChunk->lablCount > 0);

    if (tryFileData)
    {
        qDebug() << "tryFileData" << LOG_DATA;
        vector mask_from_file = readMaskFromFile(waveFile, wave.x, marker);
        vector mask_norm = normalizev(mask_from_file, MASK_MIN, MASK_MAX);
        mask = vector_resize(mask_norm, len);
        freev(mask_norm);
        freev(mask_from_file);
        qDebug() << "vector_resize" << LOG_DATA;
    }

    if (!tryFileData || !validateMask(mask)) {
        qDebug() << "!tryFileData" << LOG_DATA;
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
        if (getv(data, i) != 0 && needProcess)
        {
            needProcess = false;
            for (int j=end; j>=start; j--)
            {
                setv(data, j, getv(data, i));
            }
        }
        else if (getv(data, i) == 0 && !needProcess)
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
            setv(data, i, getv(data, start-1));
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
    qDebug() << "waveOpenFile" << LOG_DATA;

    int size = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
    qDebug() << "chunkDataSize" << LOG_DATA;
    short int bits = littleEndianBytesToUInt16(waveFile->formatChunk->significantBitsPerSample);
    qDebug() << "significantBitsPerSample" << LOG_DATA;

    vector wave = sptk_v2v(waveFile->dataChunk->waveformData, size, bits);
    qDebug() << "wave" << LOG_DATA;

    vector frame = sptk_frame(wave, sptk_settings->frame);
    qDebug() << "frame" << LOG_DATA;

    vector intensive = vector_intensive(frame, sptk_settings->frame->leng, sptk_settings->frame->shift);
    qDebug() << "intensive" << LOG_DATA;

    vector window = sptk_window(frame, sptk_settings->window);
    qDebug() << "window" << LOG_DATA;

    vector lpc = sptk_lpc(frame, sptk_settings->lpc);
    qDebug() << "lpc" << LOG_DATA;

    vector spec = sptk_spec(lpc, sptk_settings->spec);
    qDebug() << "spec " << maxv(spec) << LOG_DATA;

    vector spec_proc;
    if (sptk_settings->spec->proc == 0){
        spec_proc = vector_pow_log(spec, sptk_settings->spec->factor, sptk_settings->spec->min);
        qDebug() << "spec_log" << LOG_DATA;
    } else if (sptk_settings->spec->proc == 1){
        spec_proc = vector_pow_exp(spec, sptk_settings->spec->factor, sptk_settings->spec->min);
        qDebug() << "spec_exp" << LOG_DATA;
    }
    qDebug() << "spec_proc " << maxv(spec_proc) << LOG_DATA;

    vector pitch = processZeros(sptk_pitch_spec(wave, sptk_settings->pitch, intensive.x));
    qDebug() << "pitch" << LOG_DATA;

    vector mask = getFileMask(waveFile, wave, pitch.x);
    qDebug() << "mask" << LOG_DATA;

    vector pitch_cutted = processZeros(vector_cut_by_mask(pitch, mask));
    qDebug() << "pitch_cutted" << LOG_DATA;
    double pitch_min = getv(pitch_cutted, minv(pitch_cutted));
    double pitch_max = getv(pitch_cutted, maxv(pitch_cutted));

    vector intensive_cutted = vector_cut_by_mask(intensive, mask);
    qDebug() << "intensive_cutted" << LOG_DATA;

    vector inverted_mask = vector_invert_mask(mask);
    qDebug() << "inverted_mask" << LOG_DATA;

    vector pitch_interpolate = vector_interpolate_by_mask(
                pitch_cutted,
                inverted_mask,
                0,
                sptk_settings->plotF0->interpolation_type
                );
    qDebug() << "pitch_interpolate" << LOG_DATA;

    vector intensive_interpolate = vector_interpolate_by_mask(
                intensive_cutted,
                inverted_mask,
                0,
                sptk_settings->plotEnergy->interpolation_type
                );
    qDebug() << "intensive_interpolate" << LOG_DATA;

    vector pitch_mid = vector_smooth_mid(pitch_interpolate, sptk_settings->plotF0->frame);
    qDebug() << "pitch_mid" << LOG_DATA;

    vector intensive_mid = vector_smooth_lin(intensive_interpolate, sptk_settings->plotEnergy->frame);
    qDebug() << "intensive_mid" << LOG_DATA;

    vector norm_wave = normalizev(wave, 0.0, 1.0);

    MaskData md_p = getLabelsFromFile(waveFile, MARK_PRE_NUCLEUS);
    MaskData md_n = getLabelsFromFile(waveFile, MARK_NUCLEUS);
    MaskData md_t = getLabelsFromFile(waveFile, MARK_POST_NUCLEUS);

    vector p_mask = readMaskFromFile(waveFile, wave.x, MARK_PRE_NUCLEUS);
    qDebug() << "p_mask" << LOG_DATA;

    vector n_mask = readMaskFromFile(waveFile, wave.x, MARK_NUCLEUS);
    qDebug() << "n_mask" << LOG_DATA;

    vector t_mask = readMaskFromFile(waveFile, wave.x, MARK_POST_NUCLEUS);
    qDebug() << "t_mask" << LOG_DATA;

    vector p_wave = zero_to_nan(vector_cut_by_mask(norm_wave, p_mask));
    qDebug() << "p_mask" << LOG_DATA;

    vector n_wave = zero_to_nan(vector_cut_by_mask(norm_wave, n_mask));
    qDebug() << "n_mask" << LOG_DATA;

    vector t_wave = zero_to_nan(vector_cut_by_mask(norm_wave, t_mask));
    qDebug() << "t_mask" << LOG_DATA;

    vector pnt_mask = onesv(norm_wave.x);
    for (int i=0; i<p_mask.x && i<n_mask.x && i<t_mask.x && i<norm_wave.x; i++) {
        if (getv(p_mask, i) == 1 || getv(n_mask, i) == 1 || getv(t_mask, i) == 1)
        {
            setv(pnt_mask, i, 0);
        } else {
            setv(pnt_mask, i, 1);
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
    qDebug() << "freev" << LOG_DATA;

    file.close();
    waveCloseFile(waveFile);
    qDebug() << "waveCloseFile" << LOG_DATA;

    GraphData * data = new GraphData();

    data->d_full_wave = norm_wave;
    data->d_wave = display_wave;
    data->d_p_wave = p_wave;
    data->d_n_wave = n_wave;
    data->d_t_wave = t_wave;
    data->d_pitch_original = pitch;
    data->d_pitch = pitch_mid;
    data->pitch_max = pitch_max;
    data->pitch_min = pitch_min;
    data->d_intensive_original = intensive;
    data->d_intensive = intensive_mid;
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

vector data_spectrum(SimpleGraphData * data)
{
    if (data->b_spec == 0)
    {
        // TODO
    }
    return data->d_spec;
}

vector data_spectrum_norm(SimpleGraphData * data)
{
    if (data->b_spec_norm == 0)
    {
        vector spec = data_spectrum(data);
        data->d_spec_norm = normalizev(spec, MASK_MIN, MASK_MAX);
        data->b_spec_norm = 1;
    }
    return data->d_spec_norm;
}

vector data_cepstrum(SimpleGraphData * data)
{
    if (data->b_cepstrum == 0)
    {
        // TODO
    }
    return data->d_cepstrum;
}

vector data_cepstrum_norm(SimpleGraphData * data)
{
    if (data->b_cepstrum_norm == 0)
    {
        vector spec = data_cepstrum(data);
        data->d_cepstrum_norm = normalizev(spec, MASK_MIN, MASK_MAX);
        data->b_cepstrum_norm = 1;
    }
    return data->d_cepstrum_norm;
}

vector data_get_pitch(SimpleGraphData * data)
{
    if (data->b_pitch == 0)
    {
        // TODO
    }
    return data->d_pitch;
}

vector data_get_pitch_norm(SimpleGraphData * data)
{
    if (data->b_pitch_norm == 0)
    {
        vector pitch = data_get_pitch(data);
        data->d_pitch_norm = normalizev(pitch, MASK_MIN, MASK_MAX);
        data->b_pitch_norm = 1;
    }
    return data->d_pitch_norm;
}

vector data_get_pitch_log(SimpleGraphData * data)
{
    if (data->b_pitch_log == 0)
    {
        // TODO
    }
    return data->d_pitch_log;
}

vector data_get_pitch_derivative(SimpleGraphData * data)
{
    if (data->b_pitch_derivative == 0)
    {
        vector pitch = data_get_pitch_norm(data);
        data->d_pitch_derivative = derivativev(pitch);
        data->b_pitch_derivative == 1;
    }
    return data->d_pitch_derivative;
}

vector data_get_intensive(SimpleGraphData * data)
{
    if (data->b_intensive == 0)
    {
        SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
        data->d_intensive = vector_smooth_lin(data->d_intensive_original, sptk_settings->plotEnergy->frame);

        data->b_intensive = 1;
    }
    return data->d_intensive;
}

vector data_get_intensive_cutted(SimpleGraphData * data)
{
    if (data->b_intensive_cutted == 0)
    {
        vector intensive = data_get_intensive(data);
        vector pitch_log = data_get_pitch_log(data);
        vector resized_pitch_log = vector_resize(pitch_log, intensive.x);
        data->d_intensive_cutted = vector_cut_by_mask(intensive, resized_pitch_log);
        freev(resized_pitch_log);

        data->b_intensive_cutted = 1;
    }
    return data->d_intensive_cutted;
}

vector data_get_intensive_norm(SimpleGraphData * data)
{
    if (data->b_intensive_norm == 0)
    {
        vector intensive = data_get_intensive_cutted(data);
        data->d_intensive_norm = normalizev(intensive, MASK_MIN, MASK_MAX);
        data->b_intensive_norm == 1;
    }
    return data->d_intensive_norm;
}

vector data_get_intensive_derivative(SimpleGraphData * data)
{
    if (data->b_derivative_intensive_norm == 0)
    {
        vector intensive = data_get_intensive_norm(data);
        vector derivative_intensive = derivativev(intensive);
        data->d_derivative_intensive_norm = normalizev(derivative_intensive, MASK_MIN, MASK_MAX);
        freev(derivative_intensive);

        data->b_derivative_intensive_norm == 1;
    }
    return data->d_derivative_intensive_norm;
}

vector data_get_intensive_smooth(SimpleGraphData * data)
{
    if (data->b_intensive_smooth == 0)
    {
        SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();
        vector intensive = data_get_intensive_norm(data);

        data->d_intensive_smooth = vector_smooth_lin(intensive, sptk_settings->dp->markoutA0IntThN);
        data->b_intensive_smooth = 1;
    }
    return data->d_intensive_smooth;
}

SimpleGraphData * SimpleProcWave2Data(QString fname, bool keepWaveData)
{
    qDebug() << "::SimpleProcWave2Data" << LOG_DATA;
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    SimpleGraphData * data = new SimpleGraphData();
    data->b_pitch = 0;
    data->b_pitch_log = 0;
    data->b_pitch_derivative = 0;
    data->b_intensive = 0;
    data->b_intensive_cutted = 0;
    data->b_intensive_norm = 0;
    data->b_intensive_smooth = 0;
    data->b_derivative_intensive_norm = 0;
    data->b_spec = 0;
    data->b_cepstrum = 0;
    data->b_pitch_norm = 0;

    QFile file(fname);
    qDebug() << "::SimpleProcWave2Data QFile" << fname << LOG_DATA;
    file.open(QIODevice::ReadOnly);
    qDebug() << "::SimpleProcWave2Data file.open " << file.isOpen() << LOG_DATA;
    WaveFile * waveFile = waveOpenHFile(file.handle());
    qDebug() << "::SimpleProcWave2Data waveOpenFile" << LOG_DATA;
    data->file_data = waveFile;

    int size = littleEndianBytesToUInt32(waveFile->dataChunk->chunkDataSize);
    qDebug() << "::SimpleProcWave2Data chunkDataSize " << size << LOG_DATA;
    short int bits = littleEndianBytesToUInt16(waveFile->formatChunk->significantBitsPerSample);
    qDebug() << "::SimpleProcWave2Data significantBitsPerSample " << bits << LOG_DATA;

    double seconds = 1.0 * size / RECORD_FREQ / bits * CHAR_BIT;
    qDebug() << "::SimpleProcWave2Data seconds=" << seconds << LOG_DATA;
    data->seconds = seconds;

    vector wave = sptk_v2v(waveFile->dataChunk->waveformData, size, bits);
    qDebug() << "::SimpleProcWave2Data wave" << LOG_DATA;

    vector norm_wave = normalizev(wave, 0.0, 1.0);
    qDebug() << "::SimpleProcWave2Data norm_wave" << LOG_DATA;
    data->d_full_wave = norm_wave;

    vector frame = sptk_frame(wave, sptk_settings->frame);
    qDebug() << "::SimpleProcWave2Data frame" << LOG_DATA;

    vector intensive = vector_intensive(wave, sptk_settings->frame->leng, sptk_settings->frame->shift);
    qDebug() << "::SimpleProcWave2Data intensive" << LOG_DATA;
    data->d_intensive_original = intensive;

    vector window = sptk_window(frame, sptk_settings->window);
    qDebug() << "::SimpleProcWave2Data window" << LOG_DATA;

    vector lpc = sptk_lpc(frame, sptk_settings->lpc);
    qDebug() << "::SimpleProcWave2Data lpc " << lpc.x << LOG_DATA;

    vector lpc2c = sptk_lpc2c(lpc, sptk_settings->lpc);
    qDebug() << "::SimpleProcWave2Data lpc2c " << lpc2c.x;
    data->d_cepstrum = lpc2c;
    data->b_cepstrum = 1;

    vector spec = sptk_spec(lpc, sptk_settings->spec);
    qDebug() << "::SimpleProcWave2Data spec " << maxv(spec) << LOG_DATA;
    data->d_spec = spec;
    data->b_spec = 1;

    vector spec_proc;
    if (sptk_settings->spec->proc == 0){
        spec_proc = vector_pow_log(spec, sptk_settings->spec->factor, sptk_settings->spec->min);
        qDebug() << "::SimpleProcWave2Data spec_log" << LOG_DATA;
    } else if (sptk_settings->spec->proc == 1){
        spec_proc = vector_pow_exp(spec, sptk_settings->spec->factor, sptk_settings->spec->min);
        qDebug() << "::SimpleProcWave2Data spec_exp" << LOG_DATA;
    }
    qDebug() << "::SimpleProcWave2Data spec_proc " << maxv(spec_proc) << LOG_DATA;
    data->d_spec_proc = spec_proc;

    vector smooth_wave = vector_smooth_lin(wave, sptk_settings->dp->smooth_frame);

    vector pitch = processZeros(sptk_pitch_spec(smooth_wave, sptk_settings->pitch, intensive.x));
    qDebug() << "::SimpleProcWave2Data pitch" << LOG_DATA;
    data->d_pitch_original = pitch;

    int otype = sptk_settings->pitch->OTYPE;
    sptk_settings->pitch->OTYPE = 2;
    vector pitch_log = sptk_pitch_spec(smooth_wave, sptk_settings->pitch, intensive.x);
    sptk_settings->pitch->OTYPE = otype;
    vector pitch_log_norm = normalizev(pitch_log, MASK_MIN, MASK_MAX);
    qDebug() << "::SimpleProcWave2Data pitch_log" << LOG_DATA;
    data->d_pitch_log = pitch_log_norm;
    data->b_pitch_log = 1;

    data->d_intensive = data_get_intensive(data);

    data->d_intensive_norm = data_get_intensive_norm(data);

    vector file_mask;
    WaveFile * procFile = waveFile;
    if (sptk_settings->dp->auto_marking)
    {
        procFile = selectMarkoutAlgorithm(data);
    }
    file_mask = getFileMask(procFile, wave, pitch.x);
    data->md_p = getLabelsFromFile(procFile, MARK_PRE_NUCLEUS);
    data->md_n = getLabelsFromFile(procFile, MARK_NUCLEUS);
    data->md_t = getLabelsFromFile(procFile, MARK_POST_NUCLEUS);
    if (sptk_settings->dp->auto_marking)
    {
        waveCloseFile(procFile);
    }
    qDebug() << "::SimpleProcWave2Data file_mask" << LOG_DATA;

    vector mask_and = vector_mask_and(pitch_log_norm, file_mask);
    vector mask = vector_smooth_mid(mask_and, 10);
    qDebug() << "::SimpleProcWave2Data mask" << LOG_DATA;
    data->d_mask = mask;

    vector inverted_mask = vector_invert_mask(mask);
    qDebug() << "::SimpleProcWave2Data inverted_mask" << LOG_DATA;

    vector pitch_interpolate = vector_interpolate_by_mask(
                pitch,
                inverted_mask,
                0,
                sptk_settings->plotF0->interpolation_type
                );
    qDebug() << "::SimpleProcWave2Data pitch_interpolate" << LOG_DATA;

    vector pitch_mid = vector_smooth_mid(pitch_interpolate, sptk_settings->plotF0->frame);
    qDebug() << "::SimpleProcWave2Data pitch_mid" << LOG_DATA;
    data->d_pitch = pitch_mid;
    data->b_pitch = 1;

    data->d_derivative_intensive_norm = data_get_intensive_derivative(data);

    freev(frame);
    freev(window);
    freev(lpc);
    freev(wave);
    freev(pitch_interpolate);
    freev(inverted_mask);
    freev(smooth_wave);
    freev(pitch_log);
    freev(file_mask);
    freev(mask_and);
    qDebug() << "::SimpleProcWave2Data freev" << LOG_DATA;

    file.close();
    qDebug() << "::SimpleProcWave2Data file.close" << LOG_DATA;

    if (keepWaveData)
    {
        waveFile->file = NULL;
    } else {
        data->file_data = NULL;
        waveCloseFile(waveFile);
        qDebug() << "::SimpleProcWave2Data waveCloseFile" << LOG_DATA;
    }

    return data;
}

WaveFile * selectMarkoutAlgorithm(SimpleGraphData * data)
{
    SPTK_SETTINGS * sptk_settings = SettingsDialog::getSPTKsettings();

    if (sptk_settings->dp->markoutType == MARKOUT_F0) // F0
    {
        return markOutFileByF0(data);
    } else if (sptk_settings->dp->markoutType == MARKOUT_A0) // A0
    {
        return markOutFileByA0(data);
    } else if (sptk_settings->dp->markoutType == MARKOUT_F0A0) // F0 & A0
    {
        return markOutFileByF0A0(data);
    } else if (sptk_settings->dp->markoutType == MARKOUT_A0_INTEGRAL) // A0 Integral
    {
        return markOutFileByA0Integral(data);
    } else if (sptk_settings->dp->markoutType == MARKOUT_A0_ENVELOPE) // A0 Envelope
    {
        // TODO: A0Envelope
        return markOutFileByA0Integral(data);
    }
}

void freeGraphData(GraphData * data)
{
    freev(data->d_intensive_original);
    freev(data->d_intensive);
    freev(data->d_pitch_original);
    freev(data->d_pitch);
    freev(data->d_pitch_log);
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
    freev(data->d_pitch_original);
    freev(data->d_pitch);
    freev(data->d_pitch_log);
    freev(data->d_intensive_original);
    freev(data->d_intensive);
    freev(data->d_intensive_norm);
    if (data->b_intensive_smooth == 1)
        freev(data->d_intensive_smooth);
    freev(data->d_derivative_intensive_norm);
    freev(data->d_spec_proc);
    freev(data->d_spec);

    freeiv(data->md_p.pointsFrom);
    freeiv(data->md_p.pointsLength);
    freeiv(data->md_t.pointsFrom);
    freeiv(data->md_t.pointsLength);
    freeiv(data->md_n.pointsFrom);
    freeiv(data->md_n.pointsLength);

    if (data->file_data)
    {
        waveCloseFile(data->file_data);
    }
}
