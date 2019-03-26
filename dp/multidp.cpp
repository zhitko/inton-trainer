#include "multidp.h"

#include <QDebug>

#include "defines.h"

extern "C" {
    #include "./SPTK/others/func.h"
}

MultiDP::MultiDP(int sigSize, int pttrnSize, int limit):
    ContinuousDP(
        new SpectrSignal(zerov(pttrnSize), 1),
        new SpectrSignal(zerov(sigSize), 1),
        1,
        limit
    )
{
    qDebug() << "MultiDP::MultiDP " << sigSize << ", " << pttrnSize << ", " << limit << LOG_DATA;
    this->f0Use = false;
    this->df0Use = false;
    this->a0Use = false;
    this->da0Use = false;
    this->nmpUse = false;
    this->spectrumUse = false;
    this->cepstrumUse = false;

    this->maxF0 = 0.0;
    this->maxDF0 = 0.0;
    this->maxA0 = 0.0;
    this->maxDA0 = 0.0;
    this->maxNMP = 0.0;
    this->maxSpectrum = 0.0;
    this->maxCepstrum = 0.0;
}

void MultiDP::addF0(vector sig, vector pttrn, double coefficient)
{
    this->f0Sig = new VectorSignal(sig);
    this->f0Pttrn = new VectorSignal(pttrn);
    this->f0Coefficient = coefficient;
    this->f0Use = true;
}

void MultiDP::addDF0(vector sig, vector pttrn, double coefficient)
{
    this->df0Sig = new VectorSignal(sig);
    this->df0Pttrn = new VectorSignal(pttrn);
    this->df0Coefficient = coefficient;
    this->df0Use = true;
}

void MultiDP::addA0(vector sig, vector pttrn, double coefficient)
{
    this->a0Sig = new VectorSignal(sig);
    this->a0Pttrn = new VectorSignal(pttrn);
    this->a0Coefficient = coefficient;
    this->a0Use = true;
}

void MultiDP::addDA0(vector sig, vector pttrn, double coefficient)
{
    this->da0Sig = new VectorSignal(sig);
    this->da0Pttrn = new VectorSignal(pttrn);
    this->da0Coefficient = coefficient;
    this->da0Use = true;
}

void MultiDP::addNmp(vector sig, vector pttrn, double coefficient)
{
    this->nmpSig = new VectorSignal(sig);
    this->nmpPttrn = new VectorSignal(pttrn);
    this->nmpCoefficient = coefficient;
    this->nmpUse = true;
}

void MultiDP::addSpectrum(vector sig, vector pttrn, double coefficient, int size)
{
    this->spectrumSig = new SpectrSignal(sig, size);
    this->spectrumPttrn = new SpectrSignal(pttrn, size);
    this->spectrumCoefficient = coefficient;
    this->spectrumSize = size;
    this->spectrumUse = true;

}

void MultiDP::addCepstrum(vector sig, vector pttrn, double coefficient, int size)
{
    this->cepstrumSig = new SpectrSignal(sig, size);
    this->cepstrumPttrn = new SpectrSignal(pttrn, size);
    this->cepstrumCoefficient = coefficient;
    this->cepstrumSize = size;
    this->cepstrumUse = true;
}

double calcError(double value1, double value2)
{
    double result = 0.0;

    double y = 0.0;
    double x = 0.0;
    if (isfinite(value1))
    {
        x = value1;
    }
    if (isfinite(value2))
    {
        y = value2;
    }
    result += pow(x - y, 2.0);

    if (result > 0)
    {
        return sqrt(result);
    } else {
        return result;
    }
}

double calcVecError(double* value1, double* value2, int size)
{
    double result = 0.0;
    for(int i=0; i<size; i++)
    {
        double y = 0.0;
        double x = 0.0;
        if (isfinite(value1[i]))
        {
            x = value1[i];
        }
        if (isfinite(value2[i]))
        {
            y = value2[i];
        }
        result += pow(x - y, 2.0);
    }
    if (result > 0)
    {
        return sqrt(result/size);
    } else {
        return result;
    }
}

double MultiDP::calculateError(int value1Pos, int value2Pos)
{
//    qDebug() << "MultiDP::calculateError " << value1Pos << " - " << value2Pos << LOG_DATA;
    int count = 0;
    double error = 0.0;
    if (this->f0Use)
    {
        int value1PosUpd = (1.0 * value1Pos * this->signalSize) / this->f0Sig->size();
        int value2PosUpd = (1.0 * value2Pos * this->patternSize) / this->f0Pttrn->size();
        double value1 = this->f0Sig->valueAt(value1PosUpd);
        double value2 = this->f0Pttrn->valueAt(value2PosUpd);
        double e = calcError(value1, value2) * this->f0Coefficient;
        if (e > maxF0) maxF0 = e;
        error += e;
        count++;
    }
    if (this->df0Use)
    {
        int value1PosUpd = (1.0 * value1Pos * this->signalSize) / this->df0Sig->size();
        int value2PosUpd = (1.0 * value2Pos * this->patternSize) / this->df0Pttrn->size();
        double value1 = this->df0Sig->valueAt(value1PosUpd);
        double value2 = this->df0Pttrn->valueAt(value2PosUpd);
        double e = calcError(value1, value2) * this->df0Coefficient;
        if (e > maxDF0) maxDF0 = e;
        error += e;
        count++;
    }
    if (this->a0Use)
    {
        int value1PosUpd = (1.0 * value1Pos * this->signalSize) / this->a0Sig->size();
        int value2PosUpd = (1.0 * value2Pos * this->patternSize) / this->a0Pttrn->size();
        double value1 = this->a0Sig->valueAt(value1PosUpd);
        double value2 = this->a0Pttrn->valueAt(value2PosUpd);
        double e = calcError(value1, value2) * this->a0Coefficient;
        if (e > maxA0) maxA0 = e;
        error += e;
        count++;
    }
    if (this->da0Use)
    {
        int value1PosUpd = (1.0 * value1Pos * this->signalSize) / this->da0Sig->size();
        int value2PosUpd = (1.0 * value2Pos * this->patternSize) / this->da0Pttrn->size();
        double value1 = this->da0Sig->valueAt(value1PosUpd);
        double value2 = this->da0Pttrn->valueAt(value2PosUpd);
        double e = calcError(value1, value2) * this->da0Coefficient;
        if (e > maxDA0) maxDA0 = e;
        error += e;
        count++;
    }
    if (this->nmpUse)
    {
        int value1PosUpd = (1.0 * value1Pos * this->signalSize) / this->nmpSig->size();
        int value2PosUpd = (1.0 * value2Pos * this->patternSize) / this->nmpPttrn->size();
        double value1 = this->nmpSig->valueAt(value1PosUpd);
        double value2 = this->nmpPttrn->valueAt(value2PosUpd);
        double e = calcError(value1, value2) * this->nmpCoefficient;
        if (e > maxNMP) maxNMP = e;
        error += e;
        count++;
    }
    if (this->spectrumUse)
    {
        int value1PosUpd = (1.0 * value1Pos * this->signalSize) / this->spectrumSig->size();
        int value2PosUpd = (1.0 * value2Pos * this->patternSize) / this->spectrumPttrn->size();
        double* value1 = this->spectrumSig->valueAt(value1PosUpd);
        double* value2 = this->spectrumPttrn->valueAt(value2PosUpd);
        double e = calcVecError(value1, value2, this->spectrumSize) * this->spectrumCoefficient;
        if (e > maxSpectrum) maxSpectrum = e;
        error += e;
        count++;
    }
    if (this->cepstrumUse)
    {
        int value1PosUpd = (1.0 * value1Pos * this->signalSize) / this->cepstrumSig->size();
        int value2PosUpd = (1.0 * value2Pos * this->patternSize) / this->cepstrumPttrn->size();
        double* value1 = this->cepstrumSig->valueAt(value1PosUpd);
        double* value2 = this->cepstrumPttrn->valueAt(value2PosUpd);
        double e = calcVecError(value1, value2, this->cepstrumSize) * this->cepstrumCoefficient;
        if (e > maxCepstrum) maxCepstrum = e;
        error += e;
        count++;
    }
    //qDebug() << "MultiDP::calculateError " << (error / count) << LOG_DATA;
    return error / count;
}

