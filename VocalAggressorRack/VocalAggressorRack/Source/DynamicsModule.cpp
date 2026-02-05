/*
  ==============================================================================

    DynamicsModule.cpp
    Created: 27 Dec 2025 3:50:00pm
    Author:  Jules

  ==============================================================================
*/

#include "DynamicsModule.h"

DynamicsModule::DynamicsModule()
{
}

DynamicsModule::~DynamicsModule()
{
}

void DynamicsModule::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    smoothedGain.reset(sampleRate, 0.01); // Fast response for dynamics
}

void DynamicsModule::process(juce::AudioBuffer<float>& buffer, const PressureDetector& detector)
{
    float intensity = detector.getIntensity();
    float density = detector.getDensity();

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float targetGain = 1.0f;

        // Simple Omnipressor-style logic
        if (functionAmount < 0.25f) // Gating / De-reverb
        {
            float threshold = (0.25f - functionAmount) * 0.5f;
            targetGain = (intensity > threshold) ? 1.0f : (1.0f - sustainCut);
        }
        else if (functionAmount < 0.5f) // Expansion
        {
            targetGain = 0.5f + (intensity * 2.0f);
        }
        else if (functionAmount < 0.75f) // Compression
        {
            targetGain = 1.0f / (1.0f + (intensity * 4.0f));
        }
        else // Inversion
        {
            targetGain = 1.0f - (intensity * 1.5f);
            if (targetGain < -0.5f) targetGain = -0.5f; // Chaos!
        }

        // Apply Density influence: heavier low-freq vocals get more control
        targetGain *= (1.0f - (density * 0.2f));

        smoothedGain.setTargetValue(targetGain);
        float currentGain = smoothedGain.getNextValue();

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            buffer.setSample(channel, sample, buffer.getSample(channel, sample) * currentGain);
        }
    }
}
