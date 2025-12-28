/*
  ==============================================================================

    HarmonicsModule.h
    Created: 27 Dec 2025 4:00:00pm
    Author:  Jules

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PressureDetector.h"

class HarmonicsModule
{
public:
    HarmonicsModule();
    ~HarmonicsModule();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer, const PressureDetector& detector);

private:
    // Parameters for harmonics will go here
};
