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
    // No specific preparation needed for this simple gain processor
}

void DynamicsModule::process(juce::AudioBuffer<float>& buffer, const PressureDetector& detector)
{
    // Dynamics processing logic will go here, driven by detector.getIntensity() etc.
    // For now, let's just apply a simple gain based on intensity
    float intensity = detector.getIntensity();
    buffer.applyGain(1.0f - intensity);
}
