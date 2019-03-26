#ifndef MULTISIGNAL_H
#define MULTISIGNAL_H

#include "defines.h"

#include "dp.h"
#include "vectordp.h"
#include "spectrdp.h"
#include "continuousdp.h"

extern "C" {
    #include "vector.h"
}

class MultiDP : public ContinuousDP
{
private:
    VectorSignal * f0Sig;
    VectorSignal * f0Pttrn;
    double f0Coefficient;
    bool f0Use;

    VectorSignal * df0Sig;
    VectorSignal * df0Pttrn;
    double df0Coefficient;
    bool df0Use;

    VectorSignal * a0Sig;
    VectorSignal * a0Pttrn;
    double a0Coefficient;
    bool a0Use;

    VectorSignal * da0Sig;
    VectorSignal * da0Pttrn;
    double da0Coefficient;
    bool da0Use;

    VectorSignal * nmpSig;
    VectorSignal * nmpPttrn;
    double nmpCoefficient;
    bool nmpUse;

    SpectrSignal * spectrumSig;
    SpectrSignal * spectrumPttrn;
    double spectrumCoefficient;
    bool spectrumUse;
    int spectrumSize;

    SpectrSignal * cepstrumSig;
    SpectrSignal * cepstrumPttrn;
    double cepstrumCoefficient;
    bool cepstrumUse;
    int cepstrumSize;

public:
    MultiDP(int sigSize, int pttrnSize, int limit);

    void addF0(vector sig, vector pttrn, double coefficient);
    void addDF0(vector sig, vector pttrn, double coefficient);
    void addA0(vector sig, vector pttrn, double coefficient);
    void addDA0(vector sig, vector pttrn, double coefficient);
    void addNmp(vector sig, vector pttrn, double coefficient);
    void addSpectrum(vector sig, vector pttrn, double coefficient, int size);
    void addCepstrum(vector sig, vector pttrn, double coefficient, int size);

    double maxF0;
    double maxDF0;
    double maxA0;
    double maxDA0;
    double maxNMP;
    double maxSpectrum;
    double maxCepstrum;
protected:
    virtual double calculateError(int value1Pos, int value2Pos);
};

#endif // MULTISIGNAL_H
