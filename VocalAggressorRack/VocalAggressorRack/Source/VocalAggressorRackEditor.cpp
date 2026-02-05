#include "VocalAggressorRackEditor.h"
#include "VocalAggressorRack.h"

LevelMeter::LevelMeter(VocalAggressorRack& p) : processor(p)
{
    startTimerHz(30);
}

void LevelMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colours::black);
    g.fillRect(bounds);

    float level = processor.getCurrentLevel();
    float db = juce::Decibels::gainToDecibels(level);
    float height = juce::jmap(db, -60.0f, 0.0f, 0.0f, bounds.getHeight());

    g.setColour(juce::Colours::green.withAlpha(0.8f));
    if (db > -6.0f) g.setColour(juce::Colours::yellow.withAlpha(0.8f));
    if (db > -1.0f) g.setColour(juce::Colours::red.withAlpha(0.8f));

    g.fillRect(bounds.withTop(bounds.getHeight() - height));
}

VocalAggressorRackEditor::VocalAggressorRackEditor (VocalAggressorRack& p)
    : AudioProcessorEditor (&p), audioProcessor (p), meter(p)
{
    auto setupSlider = [this](juce::Slider& s, juce::Label& l, const juce::String& name) {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        l.setText(name, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
    };

    // Intensity
    intensitySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    addAndMakeVisible(intensitySlider);
    intensityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "intensity", intensitySlider);

    // Dynamics
    setupSlider(dynAmountSlider, dynAmountLabel, "Function");
    setupSlider(dynSustainSlider, dynSustainLabel, "Sustain");
    dynModule.addControl(dynAmountSlider, dynAmountLabel);
    dynModule.addControl(dynSustainSlider, dynSustainLabel);
    dynModule.addAndMakeVisible(dynBypassButton);
    addAndMakeVisible(dynModule);
    dynAmountAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "dyn_amount", dynAmountSlider);
    dynSustainAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "dyn_sustain", dynSustainSlider);
    dynBypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "bypass_dyn", dynBypassButton);

    // EQ
    setupSlider(eqScoopSlider, eqScoopLabel, "Scoop");
    setupSlider(eqBiteSlider, eqBiteLabel, "Bite");
    eqModule.addControl(eqScoopSlider, eqScoopLabel);
    eqModule.addControl(eqBiteSlider, eqBiteLabel);
    eqModule.addAndMakeVisible(eqBypassButton);
    addAndMakeVisible(eqModule);
    eqScoopAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "eq_scoop", eqScoopSlider);
    eqBiteAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "eq_bite", eqBiteSlider);
    eqBypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "bypass_eq", eqBypassButton);

    // Harmonics
    setupSlider(harmGritSlider, harmGritLabel, "Grit");
    setupSlider(harmClaritySlider, harmClarityLabel, "Clarity");
    harmModule.addControl(harmGritSlider, harmGritLabel);
    harmModule.addControl(harmClaritySlider, harmClarityLabel);
    harmModule.addAndMakeVisible(harmBypassButton);
    addAndMakeVisible(harmModule);
    harmGritAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "harm_grit", harmGritSlider);
    harmClarityAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "harm_clarity", harmClaritySlider);
    harmBypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "bypass_harm", harmBypassButton);

    // Shift
    setupSlider(shiftPitchSlider, shiftPitchLabel, "Pitch");
    setupSlider(shiftFormantSlider, shiftFormantLabel, "Formant");
    shiftModule.addControl(shiftPitchSlider, shiftPitchLabel);
    shiftModule.addControl(shiftFormantSlider, shiftFormantLabel);
    shiftModule.addAndMakeVisible(shiftBypassButton);
    addAndMakeVisible(shiftModule);
    shiftPitchAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "shift_pitch", shiftPitchSlider);
    shiftFormantAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "shift_formant", shiftFormantSlider);
    shiftBypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "bypass_shift", shiftBypassButton);

    // Space
    setupSlider(spaceMixSlider, spaceMixLabel, "Mix");
    setupSlider(spaceCharSlider, spaceCharLabel, "Char");
    spaceModule.addControl(spaceMixSlider, spaceMixLabel);
    spaceModule.addControl(spaceCharSlider, spaceCharLabel);
    spaceModule.addAndMakeVisible(spaceBypassButton);
    addAndMakeVisible(spaceModule);
    spaceMixAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "space_mix", spaceMixSlider);
    spaceCharAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "space_char", spaceCharSlider);
    spaceBypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "bypass_space", spaceBypassButton);

    addAndMakeVisible(meter);

    setSize (400, 700);
}

VocalAggressorRackEditor::~VocalAggressorRackEditor() {}

void VocalAggressorRackEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);
    g.setColour (juce::Colours::white);
    g.setFont (20.0f);
    g.drawText ("VOCAL AGGRESSOR RACK", getLocalBounds().removeFromTop(40), juce::Justification::centred, true);
}

void VocalAggressorRackEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(40); // Title space

    auto meterArea = area.removeFromRight(40).reduced(5);
    meter.setBounds(meterArea);

    auto mainArea = area.reduced(10);

    // Master Intensity at the top
    auto topArea = mainArea.removeFromTop(100);
    intensitySlider.setBounds(topArea.withSizeKeepingCentre(80, 80));

    // Modules stacked vertically
    int moduleHeight = mainArea.getHeight() / 5;

    auto layoutModule = [&](RackModule& m, juce::ToggleButton& b, juce::Rectangle<int> bounds) {
        m.setBounds(bounds);
        auto bArea = m.getLocalBounds().reduced(10);
        b.setBounds(bArea.removeFromRight(60).withHeight(20).withY(20));
    };

    layoutModule(dynModule, dynBypassButton, mainArea.removeFromTop(moduleHeight));
    layoutModule(eqModule, eqBypassButton, mainArea.removeFromTop(moduleHeight));
    layoutModule(harmModule, harmBypassButton, mainArea.removeFromTop(moduleHeight));
    layoutModule(shiftModule, shiftBypassButton, mainArea.removeFromTop(moduleHeight));
    layoutModule(spaceModule, spaceBypassButton, mainArea.removeFromTop(moduleHeight));
}
