#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "AudioEngine/DesiSamplerSound.h"

DesiSamplerProAudioProcessor::DesiSamplerProAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                     .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       )
#endif
{
    formatManager.registerBasicFormats();

    // Add MP3 format if enabled (it is in CMake)
    // Note: BasicFormats usually includes Wav, Aiff. MP3 might need explicit addition depending on build flags.

    for (int i = 0; i < maxVoices; i++)
        synth.addVoice (new DesiSamplerVoice());
}

DesiSamplerProAudioProcessor::~DesiSamplerProAudioProcessor()
{
}

const juce::String DesiSamplerProAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DesiSamplerProAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DesiSamplerProAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DesiSamplerProAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DesiSamplerProAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DesiSamplerProAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DesiSamplerProAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DesiSamplerProAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DesiSamplerProAudioProcessor::getProgramName (int index)
{
    return {};
}

void DesiSamplerProAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void DesiSamplerProAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
    previewTransport.prepareToPlay (samplesPerBlock, sampleRate);
}

void DesiSamplerProAudioProcessor::releaseResources()
{
    previewTransport.releaseResources();
}

bool DesiSamplerProAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
   #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
   #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
   #endif
}

void DesiSamplerProAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // LED Trigger logic
    if (!midiMessages.isEmpty())
    {
        for (const auto metadata : midiMessages)
            if (metadata.getMessage().isNoteOn())
            {
                midiActivityTrigger = true;
                break;
            }
    }

    juce::AudioSourceChannelInfo info (buffer);

    // 1. Render Preview (Background/Transport)
    // AudioTransportSource clears the buffer and fills it with source audio.
    // If stopped, it fills with silence. This acts as our "clear buffer" step too.
    previewTransport.getNextAudioBlock (info);

    // 2. Render Synth (Foreground)
    // Synthesiser::renderNextBlock adds its output to the existing buffer content.
    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());

    // MIDI Passthrough
    if (!midiPassThrough)
        midiMessages.clear(); // If not passing through, clear (but synth already used them)
}

bool DesiSamplerProAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* DesiSamplerProAudioProcessor::createEditor()
{
    return new DesiSamplerProAudioProcessorEditor (*this);
}

void DesiSamplerProAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
}

void DesiSamplerProAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
}

void DesiSamplerProAudioProcessor::loadSample(juce::File file, int octave, bool isMelodic)
{
    std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (file));
    if (reader)
    {
        juce::BigInteger notes;
        int rootNote = (octave + 1) * 12; // C3 is ~60. octave 1 -> 24. octave 4 -> 60.
        // MIDI Note 0 is C-1. Note 12 is C0. Note 24 is C1.
        // Standard convention: C4 = 60.
        // Let's assume user selection 1-8 maps to C1-C8.
        // Octave 1 starts at 24.
        int startNote = 12 * (octave + 1);

        if (isMelodic)
        {
            // Map to entire octave range (12 semitones)
            notes.setRange(startNote, 12, true);

            // Set root note to middle of that octave for 'normal' pitch,
            // OR set it to C and let it pitch up.
            // Usually melodics like Sitar are sampled at a specific root (e.g. C).
            // We assume sample is C.
            rootNote = startNote;
        }
        else
        {
            // Percussive: Map to just the Root Key (C of that octave)
            notes.setBit(startNote);
            rootNote = startNote;
        }

        // Remove existing sounds covering these notes to avoid layering chaos?
        // For now, just add on top (layering allowed as per "let me do both at same time")
        // But if we want to replace the slot...
        // Simple logic: We just add. User can clear via a "Clear" button (not implemented yet, but acceptable for MVP)

        // NOTE: This blocks the message thread while loading.
        // A production-ready version would use a background thread (e.g., TimeSliceThread) to load the file
        // and then swap the sound in a thread-safe manner.
        // Given the MVP scope, we proceed with synchronous loading but acknowledge the limitation.

        synth.addSound (new DesiSamplerSound (file.getFileName(),
                                              *reader,
                                              notes,
                                              rootNote,
                                              0.01,  // Attack
                                              0.1,   // Release
                                              10.0)); // Max length
    }
}

void DesiSamplerProAudioProcessor::playPreview(juce::File file)
{
    auto* reader = formatManager.createReaderFor (file);
    if (reader)
    {
        std::unique_ptr<juce::AudioFormatReaderSource> newSource (new juce::AudioFormatReaderSource (reader, true));
        previewTransport.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
        previewReaderSource.reset (newSource.release());
        previewTransport.start();
    }
}

// Global Factory
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DesiSamplerProAudioProcessor();
}
