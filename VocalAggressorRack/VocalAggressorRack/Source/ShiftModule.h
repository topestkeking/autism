/*
  ==============================================================================

    ShiftModule.h
    Created: 27 Dec 2025 4:05:00pm
    Author:  Jules

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PressureDetector.h"

class ShiftModule
{
public:
    ShiftModule();
    ~ShiftModule();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer, const PressureDetector& detector);

private:
    // Parameters for pitch and formant shifting will go here
};
