/*
  ==============================================================================

    PressureDetector.h
    Created: 27 Dec 2025 3:45:00pm
    Author:  Jules

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class PressureDetector
{
public:
    PressureDetector();
    ~PressureDetector();

    void process(const juce::AudioBuffer<float>& buffer);

    float getIntensity() const;
    float getDensity() const;
    float getTimbre() const;

private:
    float intensity = 0.0f;
    float density = 0.0f;
    float timbre = 0.0f;
};
