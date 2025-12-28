/*
  ==============================================================================

    VocalAggressorRack.h
    Created: 27 Dec 2025 3:30:00pm
    Author:  Jules

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             VocalAggressorRack
 version:          1.0.0
 vendor:           Jules
 website:          http://juce.com
 description:      Aggressive vocal processing rack.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_plugin_client, juce_audio_processors,
                   juce_audio_utils, juce_core, juce_data_structures,
                   juce_events, juce_graphics, juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2019, linux_make

 type:             AudioProcessor
 mainClass:        VocalAggressorRack

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

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

        dynamicsModule.prepare(spec);
        eqModule.prepare(spec);
        harmonicsModule.prepare(spec);
        shiftModule.prepare(spec);
        spaceModule.prepare(spec);
    }

    void releaseResources() override
    {
        // When playback stops, you can use this as an opportunity to free up any
        // spare memory, etc.
    }

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

    //==============================================================================
    const juce::String getName() const override                  { return JucePlugin_Name; }
    bool acceptsMidi() const override                            { return false; }
    bool producesMidi() const override                           { return false; }
    bool isMidiEffect() const override                           { return false; }
    double getTailLengthSeconds() const override                 { return 0.0; }

    //==============================================================================
    int getNumPrograms() override                                { return 1; }
    int getCurrentProgram() override                             { return 0; }
    void setCurrentProgram (int index) override                  {}
    const juce::String getProgramName (int index) override       { return {}; }
    void changeProgramName (int index, const juce::String& newName) override {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override {}
    void setStateInformation (const void* data, int sizeInBytes) override {}

private:
    //==============================================================================
    PressureDetector pressureDetector;
    DynamicsModule   dynamicsModule;
    EQModule         eqModule;
    HarmonicsModule  harmonicsModule;
    ShiftModule      shiftModule;
    SpaceModule      spaceModule;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalAggressorRack)
};
