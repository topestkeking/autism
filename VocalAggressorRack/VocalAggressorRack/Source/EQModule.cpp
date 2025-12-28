/*
  ==============================================================================

    EQModule.cpp
    Created: 27 Dec 2025 3:55:00pm
    Author:  Jules

  ==============================================================================
*/

#include "EQModule.h"

EQModule::EQModule() : sampleRate(44100.0) // Initialize with a default
{
}

EQModule::~EQModule()
{
}

void EQModule::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    lowPassFilter.prepare(spec);
    lowPassFilter.reset();
}

void EQModule::process(juce::AudioBuffer<float>& buffer, const PressureDetector& detector)
{
    float intensity = detector.getIntensity();

    // Map intensity (0.0 to 1.0) to a cutoff frequency range (e.g., 200Hz to 20kHz)
    // We'll use a logarithmic mapping for a more musical feel.
    float minFreq = 200.0f;
    float maxFreq = 20000.0f;
    float cutoff = minFreq * std::pow(maxFreq / minFreq, intensity);

    // Update the filter coefficients
    *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, cutoff);

    // Process the audio
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    lowPassFilter.process(context);
}
