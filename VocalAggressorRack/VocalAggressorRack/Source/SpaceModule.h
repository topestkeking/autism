/*
  ==============================================================================

    SpaceModule.h
    Created: 27 Dec 2025 4:10:00pm
    Author:  Jules

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PressureDetector.h"

class SpaceModule
{
public:
    SpaceModule();
    ~SpaceModule();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer, const PressureDetector& detector);

private:
    double sampleRate;
    juce::Reverb reverb;
};
