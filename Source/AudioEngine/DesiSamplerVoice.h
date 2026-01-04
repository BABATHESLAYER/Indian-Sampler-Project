#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "DesiSamplerSound.h"

class DesiSamplerVoice : public juce::SynthesiserVoice
{
public:
    DesiSamplerVoice() {}

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<DesiSamplerSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* s, int /*currentPitchWheelPosition*/) override
    {
        if (auto* sound = dynamic_cast<DesiSamplerSound*> (s))
        {
            pitchRatio = std::pow (2.0, (midiNoteNumber - sound->midiRootNote) / 12.0)
                         * sound->sourceSampleRate / getSampleRate();

            sourceSamplePosition = 0.0;
            lgain = velocity;
            rgain = velocity;

            // Simple ADSR envelope state
            adsr.setSampleRate (getSampleRate());
            adsr.setParameters ({ static_cast<float>(sound->attackTimeSecs), 0.1f, 1.0f, static_cast<float>(sound->releaseTimeSecs) });
            adsr.noteOn();

            isPlaying = true;
        }
    }

    void stopNote (float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            adsr.noteOff();
        }
        else
        {
            clearCurrentNote();
            adsr.reset();
            isPlaying = false;
        }
    }

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (!isPlaying)
            return;

        auto* sound = dynamic_cast<DesiSamplerSound*> (getCurrentlyPlayingSound().get());
        if (!sound)
            return;

        const auto& sampleData = sound->sampleData;
        const float* const inL = sampleData.getReadPointer (0);
        const float* const inR = sampleData.getNumChannels() > 1 ? sampleData.getReadPointer (1) : nullptr;

        float* outL = outputBuffer.getWritePointer (0, startSample);
         float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;

         int sampleLen = sampleData.getNumSamples();

         while (--numSamples >= 0)
         {
             auto pos = (int) sourceSamplePosition;
             auto nextPos = pos + 1;
             auto alpha = (float) (sourceSamplePosition - pos);
             auto invAlpha = 1.0f - alpha;

             if (nextPos >= sampleLen)
             {
                 stopNote (0.0f, false);
                 break;
             }

             float l = (inL[pos] * invAlpha + inL[nextPos] * alpha);
             float r = (inR ? (inR[pos] * invAlpha + inR[nextPos] * alpha) : l);

             auto envelopeValue = adsr.getNextSample();

             if (outL) *outL++ += l * lgain * envelopeValue;
             if (outR) *outR++ += r * rgain * envelopeValue;

             sourceSamplePosition += pitchRatio;

             if (!adsr.isActive())
             {
                 stopNote (0.0f, false);
                 break;
             }
         }
    }

    void pitchWheelMoved (int /*newPitchWheelValue*/) override {}
    void controllerMoved (int /*controllerNumber*/, int /*newControllerValue*/) override {}

private:
    double pitchRatio = 0.0;
    double sourceSamplePosition = 0.0;
    float lgain = 0.0f, rgain = 0.0f;
    bool isPlaying = false;
    juce::ADSR adsr;
};
