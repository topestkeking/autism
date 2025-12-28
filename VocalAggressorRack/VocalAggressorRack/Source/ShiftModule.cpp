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
    // No specific preparation needed for a pass-through
}

void ShiftModule::process(juce::AudioBuffer<float>& buffer, const PressureDetector& detector)
{
    // Pass-through: do nothing to the audio buffer.
    // The actual pitch and formant shifting logic will go here in the future.
}
