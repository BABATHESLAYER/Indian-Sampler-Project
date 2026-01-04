#pragma once

namespace juce { class AudioDeviceManager; }

/**
 * Bridge to access the Standalone Wrapper's Device Manager.
 * Returns nullptr in VST3/Plugin builds.
 * Returns valid pointer in Standalone build.
 */
juce::AudioDeviceManager* getStandaloneDeviceManager();
