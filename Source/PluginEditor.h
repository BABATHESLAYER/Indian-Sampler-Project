#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
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

    // Main Tabs
    juce::TabbedComponent mainTabs { juce::TabbedButtonBar::TabsAtTop };

    // Panel 1: Performance
    std::unique_ptr<juce::Component> performancePanel;
    GlobalLED midiLed;
    SampleBrowser browser;
    juce::MidiKeyboardComponent virtualKeyboard;

    // Panel 2: Settings
    std::unique_ptr<juce::Component> settingsPanel;
    juce::ToggleButton midiPassthroughToggle;
    std::unique_ptr<juce::AudioDeviceSelectorComponent> audioSettings;

    juce::Label titleLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DesiSamplerProAudioProcessorEditor)
};
