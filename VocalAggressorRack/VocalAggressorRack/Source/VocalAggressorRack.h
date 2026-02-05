/*
  ==============================================================================

    VocalAggressorRack.h
    Created: 27 Dec 2025 3:30:00pm
    Author:  Jules

  ==============================================================================
*/

#pragma once

#include "PressureDetector.h"
#include "DynamicsModule.h"
#include "EQModule.h"
#include "HarmonicsModule.h"
#include "ShiftModule.h"
#include "SpaceModule.h"

//==============================================================================
class VocalAggressorRack  : public juce::AudioProcessor
{
public:
    //==============================================================================
    VocalAggressorRack()
        : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                           .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
    {
    }

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = samplesPerBlock;
        spec.numChannels = getTotalNumOutputChannels();

        pressureDetector.prepare(spec);
        dynamicsModule.prepare(spec);
        eqModule.prepare(spec);
        harmonicsModule.prepare(spec);
        shiftModule.prepare(spec);
        spaceModule.prepare(spec);
    }

    void releaseResources() override {}

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override
    {
        if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
            return false;
        return true;
    }

    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        juce::ScopedNoDenormals noDenormals;
        auto totalNumInputChannels  = getTotalNumInputChannels();
        auto totalNumOutputChannels = getTotalNumOutputChannels();

        for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
            buffer.clear (i, 0, buffer.getNumSamples());

        // Update module parameters from the Master Intensity knob (simplified)
        updateParameters();

        // 1. Analyze the pressure
        pressureDetector.process(buffer);

        // 2. Process through the module chain
        dynamicsModule.process(buffer, pressureDetector);
        eqModule.process(buffer, pressureDetector);
        harmonicsModule.process(buffer, pressureDetector);
        shiftModule.process(buffer, pressureDetector);
        spaceModule.process(buffer, pressureDetector);
    }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override          { return new juce::GenericAudioProcessorEditor (*this); }
    bool hasEditor() const override                              { return true; }

    const juce::String getName() const override                  { return "Vocal Aggressor Rack"; }
    bool acceptsMidi() const override                            { return false; }
    bool producesMidi() const override                           { return false; }
    bool isMidiEffect() const override                           { return false; }
    double getTailLengthSeconds() const override                 { return 0.0; }

    int getNumPrograms() override                                { return 1; }
    int getCurrentProgram() override                             { return 0; }
    void setCurrentProgram (int index) override                  {}
    const juce::String getProgramName (int index) override       { return {}; }
    void changeProgramName (int index, const juce::String& newName) override {}

    void getStateInformation (juce::MemoryBlock& destData) override {}
    void setStateInformation (const void* data, int sizeInBytes) override {}

private:
    void updateParameters()
    {
        // The master Intensity knob drives the range of everything
        float m = masterIntensity;

        dynamicsModule.functionAmount = 0.3f + m * 0.7f;
        dynamicsModule.sustainCut = 0.2f + m * 0.8f;

        eqModule.scoopAmount = 0.5f + m * 0.5f;
        eqModule.biteAmount = 0.4f + m * 0.6f;

        harmonicsModule.gritAmount = 0.2f + m * 0.8f;
        harmonicsModule.clarityAmount = 0.3f + m * 0.7f;

        shiftModule.pitchShift = m * 2.0f;
        shiftModule.formantShift = -m * 2.0f;

        spaceModule.mixAmount = 0.1f + m * 0.4f;
        spaceModule.characterAmount = m;
    }

    //==============================================================================
    PressureDetector pressureDetector;
    DynamicsModule   dynamicsModule;
    EQModule         eqModule;
    HarmonicsModule  harmonicsModule;
    ShiftModule      shiftModule;
    SpaceModule      spaceModule;

    float masterIntensity = 0.5f; // This would be an AudioParameterFloat

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalAggressorRack)
};
