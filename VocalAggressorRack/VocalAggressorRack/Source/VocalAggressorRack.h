/*
  ==============================================================================

    VocalAggressorRack.h - Finalized Design
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

class VocalAggressorRackEditor;

//==============================================================================
class VocalAggressorRack  : public juce::AudioProcessor
{
public:
    //==============================================================================
    VocalAggressorRack()
        : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                           .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
          apvts (*this, nullptr, "Parameters", createParameterLayout())
    {
    }

    //==============================================================================
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        layout.add (std::make_unique<juce::AudioParameterFloat>  ("intensity", "Master Intensity", 0.0f, 1.0f, 0.5f));

        layout.add (std::make_unique<juce::AudioParameterFloat>  ("dyn_amount", "Dynamics Amount", 0.0f, 1.0f, 0.5f));
        layout.add (std::make_unique<juce::AudioParameterFloat>  ("dyn_sustain", "Sustain Cut", 0.0f, 1.0f, 0.5f));
        layout.add (std::make_unique<juce::AudioParameterBool>   ("bypass_dyn", "Bypass Dynamics", false));

        layout.add (std::make_unique<juce::AudioParameterFloat>  ("eq_scoop", "EQ Scoop", 0.0f, 1.0f, 0.5f));
        layout.add (std::make_unique<juce::AudioParameterFloat>  ("eq_bite", "EQ Bite", 0.0f, 1.0f, 0.5f));
        layout.add (std::make_unique<juce::AudioParameterBool>   ("bypass_eq", "Bypass EQ", false));

        layout.add (std::make_unique<juce::AudioParameterFloat>  ("harm_grit", "Harmonics Grit", 0.0f, 1.0f, 0.5f));
        layout.add (std::make_unique<juce::AudioParameterFloat>  ("harm_clarity", "Harmonics Clarity", 0.0f, 1.0f, 0.5f));
        layout.add (std::make_unique<juce::AudioParameterBool>   ("bypass_harm", "Bypass Harmonics", false));

        layout.add (std::make_unique<juce::AudioParameterFloat>  ("shift_pitch", "Pitch Shift", 0.0f, 1.0f, 0.5f));
        layout.add (std::make_unique<juce::AudioParameterFloat>  ("shift_formant", "Formant Shift", 0.0f, 1.0f, 0.5f));
        layout.add (std::make_unique<juce::AudioParameterBool>   ("bypass_shift", "Bypass Shift", false));

        layout.add (std::make_unique<juce::AudioParameterFloat>  ("space_mix", "Space Mix", 0.0f, 1.0f, 0.5f));
        layout.add (std::make_unique<juce::AudioParameterFloat>  ("space_char", "Space Character", 0.0f, 1.0f, 0.5f));
        layout.add (std::make_unique<juce::AudioParameterBool>   ("bypass_space", "Bypass Space", false));

        return layout;
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

        updateParameters();

        // 1. Analyze the pressure
        pressureDetector.process(buffer);

        // 2. Process through the module chain
        if (! *apvts.getRawParameterValue ("bypass_dyn"))
            dynamicsModule.process(buffer, pressureDetector);

        if (! *apvts.getRawParameterValue ("bypass_eq"))
            eqModule.process(buffer, pressureDetector);

        if (! *apvts.getRawParameterValue ("bypass_harm"))
            harmonicsModule.process(buffer, pressureDetector);

        if (! *apvts.getRawParameterValue ("bypass_shift"))
            shiftModule.process(buffer, pressureDetector);

        if (! *apvts.getRawParameterValue ("bypass_space"))
            spaceModule.process(buffer, pressureDetector);

        // Update level for the meter
        float maxLevel = 0.0f;
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            maxLevel = std::max(maxLevel, buffer.getMagnitude(channel, 0, buffer.getNumSamples()));

        lastLevel.set(maxLevel);
    }

    float getCurrentLevel() const { return lastLevel.get(); }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
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

    void getStateInformation (juce::MemoryBlock& destData) override
    {
        auto state = apvts.copyState();
        std::unique_ptr<juce::XmlElement> xml (state.createXml());
        copyXmlToBinary (*xml, destData);
    }

    void setStateInformation (const void* data, int sizeInBytes) override
    {
        std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
        if (xmlState.get() != nullptr)
            if (xmlState->hasTagName (apvts.state.getType()))
                apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
    }

    juce::AudioProcessorValueTreeState apvts;

private:
    void updateParameters()
    {
        // Master INTENSITY controls the range and depth of all reactive components.
        // At 0% (0.0), it provides a controlled shaper.
        // At 100% (1.0), it pushes everything into "monster" territory.
        float m = *apvts.getRawParameterValue ("intensity");
        float aggressionScale = 0.4f + (m * 1.6f); // 0.4x to 2.0x range

        dynamicsModule.functionAmount = juce::jlimit(0.0f, 1.0f, (float)*apvts.getRawParameterValue ("dyn_amount") * aggressionScale);
        dynamicsModule.sustainCut     = juce::jlimit(0.0f, 1.0f, (float)*apvts.getRawParameterValue ("dyn_sustain") * aggressionScale);

        eqModule.scoopAmount = juce::jlimit(0.0f, 1.0f, (float)*apvts.getRawParameterValue ("eq_scoop") * aggressionScale);
        eqModule.biteAmount  = juce::jlimit(0.0f, 1.0f, (float)*apvts.getRawParameterValue ("eq_bite") * aggressionScale);

        harmonicsModule.gritAmount    = juce::jlimit(0.0f, 1.0f, (float)*apvts.getRawParameterValue ("harm_grit") * aggressionScale);
        harmonicsModule.clarityAmount = juce::jlimit(0.0f, 1.0f, (float)*apvts.getRawParameterValue ("harm_clarity") * aggressionScale);

        // Pitch/Formant shifts become much more extreme as intensity rises
        float shiftScale = 1.0f + (m * 2.0f); // 1x to 3x sensitivity
        shiftModule.pitchShift   = (*apvts.getRawParameterValue ("shift_pitch") - 0.5f) * 24.0f * shiftScale;
        shiftModule.formantShift = (*apvts.getRawParameterValue ("shift_formant") - 0.5f) * 24.0f * shiftScale;

        spaceModule.mixAmount       = juce::jlimit(0.0f, 1.0f, (float)*apvts.getRawParameterValue ("space_mix") * aggressionScale);
        spaceModule.characterAmount = juce::jlimit(0.0f, 1.0f, (float)*apvts.getRawParameterValue ("space_char") * aggressionScale);
    }

    //==============================================================================
    PressureDetector pressureDetector;
    DynamicsModule   dynamicsModule;
    EQModule         eqModule;
    HarmonicsModule  harmonicsModule;
    ShiftModule      shiftModule;
    SpaceModule      spaceModule;

    juce::Atomic<float> lastLevel { 0.0f };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalAggressorRack)
};
