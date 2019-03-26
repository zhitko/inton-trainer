#ifndef DEFINES
#define DEFINES

#define LOG_FILE_NAME "logs.txt"
#define LAG_FILE_MAX_SIZE 1000000
#define LOG_DATA  "["<<__func__<<"] "<<__FILE__<<":"<<__LINE__

#define TESTING false
#define TEST(code) if (TESTING) { code; }

#define DATA_PATH "/data"
#define DATA_PATH_TRAINING "/data/PATTERNS"
#define DATA_PATH_TEST "/data/TEST"
#define DATA_PATH_USER "/data/RECORDS"
#define USER_DATA_PATH "/RECORDS/"
#define WAVE_TYPE ".wav"
#define WAVE_TYPE_FILTER "*.wav"
#define TEXT_TYPE ".txt"
#define TEXT_TYPE_FILTER "*.txt"

#define RECORD_FREQ 8000
#define SIGNIFICANT_BITS_PER_SAMPLE 16
#define NUMBER_OF_CHANNELS 1
#define CHAR_BIT_RECORD 2

#define OCTAVE_MAX_1 2.5
#define OCTAVE_MAX_2 5.0

#define DP_USE_SPECTRUM 0
#define DP_USE_CEPSTRUM 1
#define DP_USE_CEPSTRUM_A0 2
#define DP_USE_CEPSTRUM_DA0 3
#define DP_USE_CEPSTRUM_A0_DA0 4
#define DP_USE_CEPSTRUM_LOGF0 5
#define DP_USE_CEPSTRUM_A0_LOGF0 6
#define DP_USE_MULTI 7

#define MARK_PRE_NUCLEUS 'P'
#define MARK_NUCLEUS 'N'
#define MARK_POST_NUCLEUS 'T'

#define MARKOUT_PREFIX_F0 "_F0_"
#define MARKOUT_PREFIX_A0 "_A0_"
#define MARKOUT_PREFIX_F0A0 "_"

#define MASK_LEN 100
#define MASK_MIN 0.0001
#define MASK_MAX 1.0
#define MASK_LIMIT 0.5

#define NORM_FROM 0.0
#define NORM_TO   1.0

#define APP_VER "0.4.10"
#define FULL_APP_VERIOSN ("IntonTrainer (ver. " + QString(APP_VER) + " - " + QString(__DATE__) + ")")

#define MARKOUT_MODE_TEST_SAVE -2
#define MARKOUT_MODE_TEST -1
#define MARKOUT_MODE_F0 0
#define MARKOUT_MODE_A0 1
#define MARKOUT_MODE_F0A0 2

#endif // DEFINES

