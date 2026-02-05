/*
  ==============================================================================

    SpaceModule.cpp
    Created: 27 Dec 2025 4:10:00pm
    Author:  Jules

  ==============================================================================
*/

#include "SpaceModule.h"

SpaceModule::SpaceModule()
{
}

SpaceModule::~SpaceModule()
{
}

void SpaceModule::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    reverb.setSampleRate(sampleRate);
    smoothedWet.reset(sampleRate, 0.05);
}

void SpaceModule::process(juce::AudioBuffer<float>& buffer, const PressureDetector& detector)
{
    float intensity = detector.getIntensity();

    // Reverb parameters morphing
    juce::Reverb::Parameters params;

    // Size and decay increase with Intensity (Bloom effect)
    params.roomSize = juce::jlimit(0.1f, 1.0f, characterAmount * 0.5f + intensity * 0.5f);
    params.damping = 1.0f - characterAmount;
    params.width = 1.0f;

    // Ducking effect: High intensity reduces wet level slightly to keep transients clear,
    // then it "blooms" out as intensity drops.
    float targetWet = mixAmount * (0.3f + intensity * 0.7f);
    smoothedWet.setTargetValue(targetWet);

    params.wetLevel = smoothedWet.getNextValue();
    params.dryLevel = 1.0f;

    reverb.setParameters(params);

    if (buffer.getNumChannels() == 1)
    {
        reverb.processMono(buffer.getWritePointer(0), buffer.getNumSamples());
    }
    else if (buffer.getNumChannels() == 2)
    {
        reverb.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), buffer.getNumSamples());
    }
}
