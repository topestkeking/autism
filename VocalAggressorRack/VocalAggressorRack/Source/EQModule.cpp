/*
  ==============================================================================

    EQModule.cpp
    Created: 27 Dec 2025 3:55:00pm
    Author:  Jules

  ==============================================================================
*/

#include "EQModule.h"

EQModule::EQModule()
{
}

EQModule::~EQModule()
{
}

void EQModule::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    scoopFilter.prepare(spec);
    biteFilter.prepare(spec);
}

void EQModule::process(juce::AudioBuffer<float>& buffer, const PressureDetector& detector)
{
    float density = detector.getDensity();
    float timbre = detector.getTimbre();

    // 1. Dynamic Scoop (Low-Mid Mud Removal)
    // Deepen scoop when density is high or timbre is "muddy"
    float scoopGain = juce::Decibels::decibelsToGain(-24.0f * (scoopAmount * (0.5f + density + timbre * 0.5f)));
    *scoopFilter.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 300.0f, 1.0f, scoopGain);

    // 2. Dynamic Bite (High-Mid Aggression)
    // Increase bite normally, but ease off if timbre is already harsh/piercing
    float biteDrive = biteAmount * (1.5f - timbre);
    float biteGain = juce::Decibels::decibelsToGain(12.0f * biteDrive);
    *biteFilter.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 3500.0f, 0.7f, biteGain);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    scoopFilter.process(context);
    biteFilter.process(context);
}
