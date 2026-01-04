#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "StandaloneBridge.h"

DesiSamplerProAudioProcessorEditor::DesiSamplerProAudioProcessorEditor (DesiSamplerProAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    juce::LookAndFeel::setDefaultLookAndFeel (&lookAndFeel);

    // Title
    addAndMakeVisible (titleLabel);
    titleLabel.setText ("DesiSampler Pro", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (24.0f, juce::Font::bold));
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setColour (juce::Label::textColourId, juce::Colours::gold);

    // LED
    addAndMakeVisible (midiLed);

    // Browser
    addAndMakeVisible (browser);
    browser.onLoadSample = [this](juce::File f, int octave, bool isMelodic)
    {
        audioProcessor.loadSample(f, octave, isMelodic);
    };
    browser.onPreviewSample = [this](juce::File f)
    {
        audioProcessor.playPreview(f);
    };

    // Controls
    addAndMakeVisible (midiPassthroughToggle);
    midiPassthroughToggle.setButtonText ("MIDI Pass-through");
    midiPassthroughToggle.onClick = [this] { audioProcessor.midiPassThrough = midiPassthroughToggle.getToggleState(); };

    // Audio Settings (Standalone Only)
    // We access the device manager via our bridge helper.
    if (auto* deviceManager = getStandaloneDeviceManager())
    {
        audioSettings.reset (new juce::AudioDeviceSelectorComponent (*deviceManager,
                                                                     0, 256,  // Input channels
                                                                     0, 256,  // Output channels
                                                                     true,    // Midi inputs
                                                                     true,    // Midi outputs
                                                                     true,    // Chip toggle
                                                                     false)); // Hide Advanced
        addAndMakeVisible (audioSettings.get());
    }

    setSize (800, 600);
    startTimer(30); // Poll for LED
}

DesiSamplerProAudioProcessorEditor::~DesiSamplerProAudioProcessorEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel (nullptr);
}

void DesiSamplerProAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Rajasthani Palace Background Gradient
    juce::Colour woodDark = juce::Colour(0xFF3E2723);
    juce::Colour woodLight = juce::Colour(0xFF5D4037);

    juce::ColourGradient grad (woodDark, 0.0f, 0.0f, woodLight, 0.0f, (float)getHeight(), false);
    g.setGradientFill (grad);
    g.fillAll();

    // Ornate Border
    g.setColour (juce::Colours::gold);
    g.drawRect (getLocalBounds(), 4);
    g.drawRect (getLocalBounds().reduced(6), 1);
}

void DesiSamplerProAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);

    auto header = area.removeFromTop (50);
    titleLabel.setBounds (header.removeFromLeft (200));
    midiLed.setBounds (header.removeFromRight (40).reduced(5));
    midiPassthroughToggle.setBounds (header);

    if (audioSettings)
    {
        auto settingsArea = area.removeFromTop(200);
        audioSettings->setBounds(settingsArea);
        area.removeFromTop(10);
    }

    browser.setBounds (area);
}

void DesiSamplerProAudioProcessorEditor::timerCallback()
{
    if (audioProcessor.midiActivityTrigger.exchange(false))
    {
        midiLed.triggerFlash();
    }
}
