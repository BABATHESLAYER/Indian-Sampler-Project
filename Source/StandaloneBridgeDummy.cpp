#include "StandaloneBridge.h"
#include <juce_audio_devices/juce_audio_devices.h>

juce::AudioDeviceManager* getStandaloneDeviceManager()
{
    // In VST3/AU/etc, we do not own the device manager.
    return nullptr;
}
