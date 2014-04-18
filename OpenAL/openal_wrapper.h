#ifndef OPENAL
#define OPENAL

#include <AL/al.h>
#include <AL/alc.h>

typedef enum _oal_device_state {
    device_failed,
    device_founed,
    device_init,
    device_
} oal_device_state;

typedef struct {
    ALCchar *name;
    ALCdevice *device;
    ALCcontext *context;
} oal_device;

typedef struct _oal_devices_list {
    oal_device *device;
    struct _oal_devices_list * next;
} oal_devices_list;

void printListAudioDevices(const oal_devices_list *);

oal_devices_list * getInputDevices();
oal_devices_list * getOutputDevices();

// Output audio device functions
int initAudioOutputDevice(oal_device*);
int freeAudioOutputDevice(oal_device*);

// Input audio device functions
int initAudioInputDevice(oal_device*);
int startCapture(oal_device*);
int getSample(oal_device*, void*, int, int);
int stopCapture(oal_device*);
int freeAudioInputDevice(oal_device*);

#endif
