#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include "AudioEngine/DesiSamplerVoice.h"

class DesiSamplerProAudioProcessor  : public juce::AudioProcessor
{
public:
    DesiSamplerProAudioProcessor();
    ~DesiSamplerProAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void loadSample(juce::File file, int octave, bool isMelodic);
    void playPreview(juce::File file);

    // Public for UI access (simplification)
    std::atomic<bool> midiActivityTrigger { false };
    bool midiPassThrough = false;

private:
    juce::Synthesiser synth;
    juce::AudioFormatManager formatManager;

    // Preview
    juce::AudioTransportSource previewTransport;
    std::unique_ptr<juce::AudioFormatReaderSource> previewReaderSource;

    // Polyphony
    static constexpr int maxVoices = 8;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DesiSamplerProAudioProcessor)
};
