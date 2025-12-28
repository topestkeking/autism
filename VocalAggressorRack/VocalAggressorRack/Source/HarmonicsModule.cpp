/*
  ==============================================================================

    HarmonicsModule.cpp
    Created: 27 Dec 2025 4:00:00pm
    Author:  Jules

  ==============================================================================
*/

#include "HarmonicsModule.h"

HarmonicsModule::HarmonicsModule()
{
}

HarmonicsModule::~HarmonicsModule()
{
}

void HarmonicsModule::prepare(const juce::dsp::ProcessSpec& spec)
{
    // No specific preparation needed for this simple waveshaper
}

void HarmonicsModule::process(juce::AudioBuffer<float>& buffer, const PressureDetector& detector)
{
    float intensity = detector.getIntensity();

    // Use intensity to drive the input gain before the waveshaper.
    // Map intensity (0-1) to a gain factor (e.g., 1 to 10).
    float drive = 1.0f + intensity * 9.0f;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float inputSample = channelData[sample] * drive;
            // Basic waveshaping using std::tanh for a soft-clipping effect
            channelData[sample] = std::tanh(inputSample);
        }
    }
}
