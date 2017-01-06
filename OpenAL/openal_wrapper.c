#include "openal_wrapper.h"

#include <stdio.h>
#include <string.h>

oal_device * mkAolDevice()
{
    oal_device * device = (oal_device *) malloc(sizeof(oal_device));
    device->device = NULL;
    device->name = NULL;
    return device;
}

oal_devices_list * ALCchar2DevicesList(const ALCchar *devices)
{
    oal_devices_list * first = NULL, * current = NULL;
    size_t len = 0;
    if(!devices || *devices == '\0')
        printf("!!! none !!!\n");
    else do {
        oal_devices_list * nextDev = (oal_devices_list *) malloc(sizeof(oal_devices_list));
        if(current != NULL)
            current->next = nextDev;
        else
            first = nextDev;
        current = nextDev;

        len = strlen(devices);
        current->device = mkAolDevice();
        current->device->name = (ALCchar *) calloc(len+1, sizeof(ALCchar));
        current->next = NULL;

        strncpy(current->device->name, devices, len);
        devices += len + 1;
    } while(*devices != '\0');

    return first;
}

void printListAudioDevices(const oal_devices_list * devices)
{
    const oal_devices_list *device = devices;

    fprintf(stdout, "Devices list:\n");
    fprintf(stdout, "----------\n");
    while (device)
    {
        fprintf(stdout, "%s\n", device->device->name);
        device = device->next;
    }
    fprintf(stdout, "----------\n");
}

oal_devices_list * getInputDevices()
{
    return ALCchar2DevicesList(alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER));
}

oal_devices_list * getOutputDevices()
{
    if(alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT") != AL_FALSE)
        return ALCchar2DevicesList(alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER));
    else
        return ALCchar2DevicesList(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
}

int initAudioOutputDevice(oal_device * device)
{
    if(device->name); else return 0;
    if(device->device = alcOpenDevice(device->name)); else return 0;
    return 1;
}

int isAudioOk()
{
    return (alGetError() == AL_NO_ERROR);
}

ALenum to_al_format(short channels, short samples)
{
        switch (samples) {
            case 16:
                    if (channels > 1) return AL_FORMAT_STEREO16;
                    else return AL_FORMAT_MONO16;
            case 8:
                    if (channels > 1) return AL_FORMAT_STEREO8;
                    else return AL_FORMAT_MONO8;
            default: return -1;
        }
}

int playSound(oal_device * device, void * data, int size, short channels, short samples, int sampleRate)
{
    const ALCint* attrlist = NULL;

    ALCcontext *context;
    ALuint buffer;
    ALuint source;
    ALint source_state;

    context = alcCreateContext(device->device, attrlist);
    alcMakeContextCurrent(context);

    alGenBuffers((ALuint)1, &buffer);
    alBufferData(buffer, to_al_format(channels, samples), data, size, sampleRate);

    alGenSources((ALuint)1, &source);
    alSourcef(source, AL_PITCH, 1);
    alSourcef(source, AL_GAIN, 1);
    alSource3f(source, AL_POSITION, 0, 0, 0);
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    alSourcei(source, AL_LOOPING, AL_FALSE);
    alSourcei(source, AL_BUFFER, buffer);

    alSourcePlay(source);
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    while (source_state == AL_PLAYING) {
        alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    }

    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    device = alcGetContextsDevice(context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);

    return 1;
}

int freeAudioOutputDevice(oal_device * device)
{
    if(device->device) alcCloseDevice(device->device);
    return 1;
}

int initAudioInputDevice(oal_device * device)
{
    if(device->name); else return 0;
    if(device->device = alcCaptureOpenDevice(device->name, 8000, AL_FORMAT_MONO16, 4096)); else return 0;
    return 1;
}

int startCapture(oal_device * device)
{
    alcCaptureStart(device->device);
    return 1;
}

int getSample(oal_device * device, void * buffer, int size, int max)
{
    ALint samples;
    ALint maxSamples = max / size;
    alcGetIntegerv(device->device, ALC_CAPTURE_SAMPLES, (ALCsizei)size, &samples);
    if(samples > maxSamples) samples = maxSamples;
    if(samples != 0)
    {
        alcCaptureSamples(device->device, (ALCvoid *)buffer, samples);
        return (int) samples * size;
    }else
        return 0;
}

int stopCapture(oal_device * device)
{
    if(device->device) alcCaptureStop(device->device);
    return 1;
}

int freeAudioInputDevice(oal_device * device)
{
    if(device->device) alcCaptureCloseDevice(device->device);
    return 1;
}
