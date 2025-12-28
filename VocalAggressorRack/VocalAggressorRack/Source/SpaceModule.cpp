/*
  ==============================================================================

    SpaceModule.cpp
    Created: 27 Dec 2025 4:10:00pm
    Author:  Jules

  ==============================================================================
*/

#include "SpaceModule.h"

SpaceModule::SpaceModule() : sampleRate(44100.0) // Initialize with a default
{
}

SpaceModule::~SpaceModule()
{
}

void SpaceModule::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    reverb.setSampleRate(sampleRate);
}

void SpaceModule::process(juce::AudioBuffer<float>& buffer, const PressureDetector& detector)
{
    float intensity = detector.getIntensity();

    // Create a mono buffer to process the reverb, as juce::Reverb works on a mono signal.
    juce::AudioBuffer<float> monoBuffer(1, buffer.getNumSamples());
    monoBuffer.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());
    if (buffer.getNumChannels() > 1)
    {
        monoBuffer.addFrom(0, 0, buffer, 1, 0, buffer.getNumSamples());
        monoBuffer.applyGain(0.5f);
    }

    // Set reverb parameters. We'll make the mix dynamic.
    juce::Reverb::Parameters params;
    params.roomSize = 0.8f;
    params.damping = 0.5f;
    params.wetLevel = intensity; // Direct mapping of intensity to wet level
    params.dryLevel = 1.0f - intensity;
    params.width = 1.0f;
    reverb.setParameters(params);

    // Process the mono buffer through the reverb
    reverb.processMono(monoBuffer.getWritePointer(0), monoBuffer.getNumSamples());

    // Add the reverberated signal back to the main buffer
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        buffer.addFrom(channel, 0, monoBuffer, 0, 0, monoBuffer.getNumSamples(), 1.0f);
    }
}
