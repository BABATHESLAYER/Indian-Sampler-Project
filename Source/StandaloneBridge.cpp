#include "StandaloneBridge.h"
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h>

juce::AudioDeviceManager* getStandaloneDeviceManager()
{
    // Access the singleton provided by the JUCE Standalone wrapper
    if (auto* holder = juce::StandalonePluginHolder::getInstance())
    {
        return &holder->deviceManager;
    }
    return nullptr;
}
