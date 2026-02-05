#pragma once

#include <JuceHeader.h>

class VocalAggressorRack; // Forward declaration

//==============================================================================
class LevelMeter : public juce::Component, public juce::Timer
{
public:
    LevelMeter(VocalAggressorRack& p);
    void paint(juce::Graphics& g) override;
    void timerCallback() override { repaint(); }

private:
    VocalAggressorRack& processor;
};

//==============================================================================
class RackModule : public juce::GroupComponent
{
public:
    RackModule(const juce::String& name) : juce::GroupComponent({}, name)
    {
        setTextLabelPosition(juce::Justification::centredTop);
    }

    void addControl(juce::Component& c, juce::Component& label)
    {
        addAndMakeVisible(c);
        addAndMakeVisible(label);
        controls.add(&c);
        labels.add(&label);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(10);
        area.removeFromTop(20); // Room for title

        int count = controls.size();
        if (count == 0) return;

        int width = area.getWidth() / count;
        for (int i = 0; i < count; ++i)
        {
            auto cArea = area.removeFromLeft(width);
            labels[i]->setBounds(cArea.removeFromBottom(20));
            controls[i]->setBounds(cArea.reduced(5));
        }
    }

private:
    juce::Array<juce::Component*> controls;
    juce::Array<juce::Component*> labels;
};

//==============================================================================
class VocalAggressorRackEditor  : public juce::AudioProcessorEditor
{
public:
    VocalAggressorRackEditor (VocalAggressorRack&);
    ~VocalAggressorRackEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    VocalAggressorRack& audioProcessor;

    juce::Slider intensitySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> intensityAttachment;

    RackModule dynModule { "DYNAMICS" };
    juce::Slider dynAmountSlider, dynSustainSlider;
    juce::Label dynAmountLabel, dynSustainLabel;
    juce::ToggleButton dynBypassButton { "Bypass" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dynAmountAttach, dynSustainAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> dynBypassAttach;

    RackModule eqModule { "EQ" };
    juce::Slider eqScoopSlider, eqBiteSlider;
    juce::Label eqScoopLabel, eqBiteLabel;
    juce::ToggleButton eqBypassButton { "Bypass" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqScoopAttach, eqBiteAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> eqBypassAttach;

    RackModule harmModule { "HARMONICS" };
    juce::Slider harmGritSlider, harmClaritySlider;
    juce::Label harmGritLabel, harmClarityLabel;
    juce::ToggleButton harmBypassButton { "Bypass" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> harmGritAttach, harmClarityAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> harmBypassAttach;

    RackModule shiftModule { "SHIFT" };
    juce::Slider shiftPitchSlider, shiftFormantSlider;
    juce::Label shiftPitchLabel, shiftFormantLabel;
    juce::ToggleButton shiftBypassButton { "Bypass" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> shiftPitchAttach, shiftFormantAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> shiftBypassAttach;

    RackModule spaceModule { "SPACE" };
    juce::Slider spaceMixSlider, spaceCharSlider;
    juce::Label spaceMixLabel, spaceCharLabel;
    juce::ToggleButton spaceBypassButton { "Bypass" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> spaceMixAttach, spaceCharAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> spaceBypassAttach;

    LevelMeter meter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalAggressorRackEditor)
};
