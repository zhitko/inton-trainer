#ifndef DEFINES
#define DEFINES

#define LOG_FILE_NAME "logs.txt"
#define LAG_FILE_MAX_SIZE 1000000
#define LOG_DATA  "["<<__func__<<"] "<<__FILE__<<":"<<__LINE__

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

#define MARK_PRE_NUCLEUS 'P'
#define MARK_NUCLEUS 'N'
#define MARK_POST_NUCLEUS 'T'

#define MASK_LEN 100
#define MASK_MIN 0.0001
#define MASK_MAX 1.0

#define APP_VER "0.2.3"

#endif // DEFINES

