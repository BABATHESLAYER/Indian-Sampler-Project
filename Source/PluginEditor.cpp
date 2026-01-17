#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "StandaloneBridge.h"

DesiSamplerProAudioProcessorEditor::DesiSamplerProAudioProcessorEditor (DesiSamplerProAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), virtualKeyboard (p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    juce::LookAndFeel::setDefaultLookAndFeel (&lookAndFeel);

    // --- Performance Panel ---
    performancePanel = std::make_unique<juce::Component>();
    performancePanel->addAndMakeVisible(browser);
    performancePanel->addAndMakeVisible(virtualKeyboard);
    performancePanel->addAndMakeVisible(midiLed);

    browser.onLoadSample = [this](juce::File f, int octave, bool isMelodic)
    {
        audioProcessor.loadSample(f, octave, isMelodic);
    };
    browser.onPreviewSample = [this](juce::File f)
    {
        audioProcessor.playPreview(f);
    };

    // --- Settings Panel ---
    settingsPanel = std::make_unique<juce::Component>();
    settingsPanel->addAndMakeVisible(midiPassthroughToggle);
    midiPassthroughToggle.setButtonText ("MIDI Pass-through");
    midiPassthroughToggle.onClick = [this] { audioProcessor.midiPassThrough = midiPassthroughToggle.getToggleState(); };

    // Audio Settings (Standalone Only)
    if (auto* deviceManager = getStandaloneDeviceManager())
    {
        audioSettings.reset (new juce::AudioDeviceSelectorComponent (*deviceManager,
                                                                     0, 256,
                                                                     0, 256,
                                                                     true,
                                                                     true,
                                                                     true,
                                                                     false));
        settingsPanel->addAndMakeVisible (audioSettings.get());
    }

    // --- Main Tabs ---
    addAndMakeVisible(mainTabs);
    mainTabs.addTab("Play", juce::Colours::transparentBlack, performancePanel.get(), false);
    mainTabs.addTab("Settings", juce::Colours::transparentBlack, settingsPanel.get(), false);

    // --- Header Elements (Overlay or inside tabs?) ---
    // We'll keep the Title separate above tabs
    addAndMakeVisible (titleLabel);
    titleLabel.setText ("DESI SAMPLER PRO // CYBER_EDIT", juce::dontSendNotification);
    titleLabel.setFont (juce::FontOptions(24.0f, juce::Font::bold));
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    titleLabel.setColour (juce::Label::textColourId, juce::Colour(0xFFFFD700));

    setSize (900, 700);
    startTimer(30);
}

DesiSamplerProAudioProcessorEditor::~DesiSamplerProAudioProcessorEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel (nullptr);
}

void DesiSamplerProAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF1E1E1E)); // Dark background
}

void DesiSamplerProAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(10);

    // Title Bar
    auto header = area.removeFromTop (40);
    titleLabel.setBounds (header);

    // Tabs take rest
    mainTabs.setBounds(area);

    // -- Performance Layout --
    if (performancePanel)
    {
        auto pArea = performancePanel->getLocalBounds().reduced(15);
        auto ledArea = pArea.removeFromTop(30).removeFromRight(30);
        midiLed.setBounds(ledArea);

        auto keyArea = pArea.removeFromBottom(120);
        pArea.removeFromBottom(10);
        virtualKeyboard.setBounds(keyArea);
        browser.setBounds(pArea);
    }

    // -- Settings Layout --
    if (settingsPanel)
    {
        auto sArea = settingsPanel->getLocalBounds().reduced(20);
        midiPassthroughToggle.setBounds(sArea.removeFromTop(30));
        sArea.removeFromTop(10);

        if (audioSettings)
            audioSettings->setBounds(sArea);
    }
}

void DesiSamplerProAudioProcessorEditor::timerCallback()
{
    if (audioProcessor.midiActivityTrigger.exchange(false))
    {
        midiLed.triggerFlash();
    }
}
