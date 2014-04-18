#include "openal_wrapper.h"

#include <stdio.h>
#include <string.h>

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
        current->device = (oal_device *) malloc(sizeof(oal_device));
        current->device->name = (ALCchar *) calloc(len+1, sizeof(ALCchar));
        current->next = NULL;

        strncpy(current->device->name, devices, len);

        printf("%s\n", devices);
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
    const ALCint* attrlist = NULL;
    if(device->name); else return 0;
    if(device->device = alcOpenDevice(device->name)); else return 0;
    if(device->context = alcCreateContext(device->device, attrlist)); else return 0;
    if(alcMakeContextCurrent(device->context)); else return 0;
    alListener3f(AL_POSITION, 0, 0, 0);
    alListener3f(AL_VELOCITY, 0, 0, 0);
    alListener3f(AL_ORIENTATION, 0, 0, -1);
    return 1;
}

int freeAudioOutputDevice(oal_device * device)
{
    alcMakeContextCurrent(NULL);
    if(device->context) alcDestroyContext(device->context);
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
