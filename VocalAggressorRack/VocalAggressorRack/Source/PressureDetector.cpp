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

void PressureDetector::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;

    juce::dsp::ProcessSpec monoSpec = spec;
    monoSpec.numChannels = 1;

    lowFilter.prepare(monoSpec);
    midFilter.prepare(monoSpec);
    highFilter.prepare(monoSpec);

    // Low band for "Density" (e.g., up to 400Hz)
    *lowFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 400.0f);

    // Mid band for "Timbre" analysis (e.g., 2kHz to 5kHz)
    *midFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, 3500.0f, 0.5f);

    // High band for "Harshness"
    *highFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 6000.0f);

    smoothedIntensity.reset(sampleRate, 0.05);
    smoothedDensity.reset(sampleRate, 0.1);
    smoothedTimbre.reset(sampleRate, 0.1);
}

void PressureDetector::process(const juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumSamples() == 0) return;

    // 1. Intensity: Overall RMS
    float rawIntensity = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
    smoothedIntensity.setTargetValue(juce::jlimit(0.0f, 1.0f, rawIntensity * 2.0f)); // Normalized/boosted
    intensity = smoothedIntensity.getNextValue();

    // Create a mono copy for spectral analysis
    juce::AudioBuffer<float> mono(1, buffer.getNumSamples());
    mono.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());
    if (buffer.getNumChannels() > 1) {
        mono.addFrom(0, 0, buffer, 1, 0, buffer.getNumSamples());
        mono.applyGain(0.5f);
    }

    // 2. Density: Low energy vs mid/high energy
    juce::AudioBuffer<float> lowPassBuffer(1, buffer.getNumSamples());
    lowPassBuffer.copyFrom(0, 0, mono, 0, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(lowPassBuffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    lowFilter.process(context);

    float lowRMS = lowPassBuffer.getRMSLevel(0, 0, buffer.getNumSamples());
    float totalRMS = mono.getRMSLevel(0, 0, buffer.getNumSamples()) + 0.0001f;

    // Density is the ratio of low-frequency energy to total energy
    smoothedDensity.setTargetValue(juce::jlimit(0.0f, 1.0f, lowRMS / totalRMS));
    density = smoothedDensity.getNextValue();

    // 3. Timbre: High-mid harshness
    juce::AudioBuffer<float> highMidBuffer(1, buffer.getNumSamples());
    highMidBuffer.copyFrom(0, 0, mono, 0, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> midBlock(highMidBuffer);
    juce::dsp::ProcessContextReplacing<float> midContext(midBlock);
    midFilter.process(midContext);

    float midRMS = highMidBuffer.getRMSLevel(0, 0, buffer.getNumSamples());

    // Timbre here represents the presence of harsh resonant energy in the mids
    smoothedTimbre.setTargetValue(juce::jlimit(0.0f, 1.0f, midRMS / (totalRMS * 0.5f)));
    timbre = smoothedTimbre.getNextValue();
}

float PressureDetector::getIntensity() const { return intensity; }
float PressureDetector::getDensity() const   { return density; }
float PressureDetector::getTimbre() const    { return timbre; }
