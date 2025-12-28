/*
  ==============================================================================

    DynamicsModule.h
    Created: 27 Dec 2025 3:50:00pm
    Author:  Jules

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PressureDetector.h"

class DynamicsModule
{
public:
    DynamicsModule();
    ~DynamicsModule();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer, const PressureDetector& detector);

private:
    // Parameters for dynamics will go here
};
