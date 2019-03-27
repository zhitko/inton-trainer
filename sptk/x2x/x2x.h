#if defined(WIN32)
#  include "vector.h"
#else
#  include "../vector.h"
#endif

vector sptk_x2x(const char *fname);
vector sptk_v2v(void *data, int byteSize, short int bitDepth);
