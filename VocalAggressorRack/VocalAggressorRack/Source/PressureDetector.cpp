/*
  ==============================================================================

    PressureDetector.cpp
    Created: 27 Dec 2025 3:45:00pm
    Author:  Jules

  ==============================================================================
*/

#include "PressureDetector.h"

PressureDetector::PressureDetector()
{
}

PressureDetector::~PressureDetector()
{
}

void PressureDetector::process(const juce::AudioBuffer<float>& buffer)
{
    // RMS calculation for intensity will go here
    intensity = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
}

float PressureDetector::getIntensity() const
{
    return intensity;
}

float PressureDetector::getDensity() const
{
    // Spectral analysis for density will go here
    return density;
}

float PressureDetector::getTimbre() const
{
    // Timbre analysis will go here
    return timbre;
}
