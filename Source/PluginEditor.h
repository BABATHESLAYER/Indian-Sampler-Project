#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "UI/DesiLookAndFeel.h"
#include "UI/GlobalLED.h"
#include "UI/SampleBrowser.h"

class DesiSamplerProAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    DesiSamplerProAudioProcessorEditor (DesiSamplerProAudioProcessor&);
    ~DesiSamplerProAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    DesiSamplerProAudioProcessor& audioProcessor;

    DesiLookAndFeel lookAndFeel;
    GlobalLED midiLed;
    SampleBrowser browser;

    juce::Label titleLabel;
    juce::GroupComponent ioGroup;
    juce::ToggleButton midiPassthroughToggle;

    // Audio Settings for Standalone
    std::unique_ptr<juce::AudioDeviceSelectorComponent> audioSettings;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DesiSamplerProAudioProcessorEditor)
};
