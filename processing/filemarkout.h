#ifndef FILEMARKOUT_H
#define FILEMARKOUT_H


#include "dataprocessing.h"

WaveFile * markOutFileByF0(SimpleGraphData *data);
WaveFile * markOutFileByA0(SimpleGraphData *data);
WaveFile * markOutFileByF0A0(SimpleGraphData *data);

#endif // FILEMARKOUT_H
