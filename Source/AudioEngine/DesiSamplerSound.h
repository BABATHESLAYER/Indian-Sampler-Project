#pragma once
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>

class DesiSamplerSound : public juce::SynthesiserSound
{
public:
    DesiSamplerSound (const juce::String& soundName,
                      juce::AudioFormatReader& sourceReader,
                      const juce::BigInteger& midiNoteSet,
                      int midiRoot,
                      double attack,
                      double release,
                      double maxLen)
        : name (soundName),
          midiNotes (midiNoteSet),
          midiRootNote (midiRoot),
          attackTimeSecs (attack),
          releaseTimeSecs (release),
          maxSampleLengthSeconds (maxLen)
    {
        // Load entire sample into memory for real-time safety
        int numLen = (int) sourceReader.lengthInSamples;
        if (maxLen > 0) numLen = std::min(numLen, (int)(sourceReader.sampleRate * maxLen));

        sampleData.setSize(sourceReader.numChannels, numLen);
        sourceReader.read(&sampleData, 0, numLen, 0, true, true);
    }

    bool appliesToNote (int midiNoteNumber) override
    {
        return midiNotes[midiNoteNumber];
    }

    bool appliesToChannel (int /*midiChannel*/) override
    {
        return true;
    }

    juce::String name;
    juce::AudioBuffer<float> sampleData; // Replaced reference with actual buffer
    double sourceSampleRate;
    juce::BigInteger midiNotes;
    int midiRootNote;
    double attackTimeSecs, releaseTimeSecs, maxSampleLengthSeconds;
};
