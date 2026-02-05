/*
  ==============================================================================

    ShiftModule.cpp
    Created: 27 Dec 2025 4:05:00pm
    Author:  Jules

  ==============================================================================
*/

#include "ShiftModule.h"

ShiftModule::ShiftModule()
{
}

ShiftModule::~ShiftModule()
{
}

void ShiftModule::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    delayLines.resize(spec.numChannels);
    for (auto& dl : delayLines)
        dl.setup(8192);
}

void ShiftModule::process(juce::AudioBuffer<float>& buffer, const PressureDetector& detector)
{
    float intensity = detector.getIntensity();

    // Automatic "bloom": as intensity increases, shift formant down and pitch up (as requested)
    float dynamicFormant = formantShift - (intensity * 5.0f);
    float dynamicPitch = pitchShift + (intensity * 2.0f);

    // This is a very primitive "unstable" pitch shifter for character.
    // It will produce artifacts, which fits the "aggressive/unstable" design.
    float ratio = std::pow(2.0f, (dynamicPitch + dynamicFormant) / 12.0f);
    float delayRange = 500.0f; // samples

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        phase += (1.0f - ratio);
        if (phase >= delayRange) phase -= delayRange;
        if (phase < 0) phase += delayRange;

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            float input = buffer.getSample(channel, sample);
            delayLines[channel].write(input);

            // Simple dual-tap crossfade to hide the wrap-around (slightly)
            float out1 = delayLines[channel].read(phase);
            float out2 = delayLines[channel].read(std::fmod(phase + delayRange * 0.5f, delayRange));

            float fade = std::abs((phase / delayRange) - 0.5f) * 2.0f;
            float output = out1 * fade + out2 * (1.0f - fade);

            buffer.setSample(channel, sample, output);
        }
    }
}
