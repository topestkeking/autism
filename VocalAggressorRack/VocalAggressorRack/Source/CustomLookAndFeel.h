/*
  ==============================================================================

    CustomLookAndFeel.h - Professional Industry-Standard UI/UX
    Custom drawing for knobs, buttons, and meters.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        setColour (juce::Slider::thumbColourId, juce::Colour (0xffff6600));
        setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xffff6600));
        setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colours::black);
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override
    {
        auto outline = slider.findColour (juce::Slider::rotarySliderOutlineColourId);
        auto fill    = slider.findColour (juce::Slider::rotarySliderFillColourId);

        auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (10);

        auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = 3.0f;
        auto arcRadius = radius - lineW * 0.5f;

        // Background Path
        juce::Path backgroundArc;
        backgroundArc.addCentredArc (bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);

        // Shadow for the track
        g.setColour (juce::Colours::black.withAlpha(0.3f));
        g.strokePath (backgroundArc, juce::PathStrokeType (lineW + 1.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        g.setColour (outline.withAlpha(0.2f));
        g.strokePath (backgroundArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        if (slider.isEnabled())
        {
            // Value Path
            juce::Path valueArc;
            valueArc.addCentredArc (bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius, 0.0f, rotaryStartAngle, toAngle, true);

            // Glow effect for the value arc
            g.setColour (fill.withAlpha(0.3f));
            g.strokePath (valueArc, juce::PathStrokeType (lineW + 2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

            g.setColour (fill);
            g.strokePath (valueArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        // Knob body
        auto knobRadius = radius - 7.0f;
        juce::Rectangle<float> knobBounds (bounds.getCentreX() - knobRadius, bounds.getCentreY() - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);

        // Metallic brushed texture effect
        juce::ColourGradient grad (juce::Colour (0xff444444), knobBounds.getCentreX(), knobBounds.getY(),
                                   juce::Colour (0xff1a1a1a), knobBounds.getCentreX(), knobBounds.getBottom(), false);
        g.setGradientFill (grad);
        g.fillEllipse (knobBounds);

        // Subtle inner highlight
        g.setColour (juce::Colours::white.withAlpha (0.05f));
        g.drawEllipse (knobBounds.reduced(0.5f), 1.5f);

        // Outer rim
        g.setColour (juce::Colours::black.withAlpha(0.8f));
        g.drawEllipse (knobBounds, 1.0f);

        // Indicator dot
        if (slider.isEnabled())
        {
            juce::Path p;
            auto dotSize = 4.0f;
            auto dotRadius = knobRadius - dotSize * 2.0f;
            p.addEllipse (-dotSize * 0.5f, -dotRadius, dotSize, dotSize);
            p.applyTransform (juce::AffineTransform::rotation (toAngle).translated (bounds.getCentreX(), bounds.getCentreY()));
            g.setColour (fill);
            g.fillPath (p);

            // Glow
            g.setColour (fill.withAlpha(0.3f));
            g.drawEllipse (p.getBounds().expanded(2.0f), 1.0f);
        }
    }

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto fontSize = juce::jmin (15.0f, button.getHeight() * 0.75f);
        auto tickWidth = fontSize * 1.1f;

        drawTickBox (g, button, 4.0f, (button.getHeight() - tickWidth) * 0.5f, tickWidth, tickWidth,
                     button.getToggleState(), button.isEnabled(), shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

        g.setColour (button.findColour (juce::ToggleButton::textColourId));
        g.setFont (fontSize);

        if (! button.isEnabled())
            g.setOpacity (0.5f);

        g.drawText (button.getButtonText(), button.getLocalBounds().withTrimmedLeft (roundToInt (tickWidth * 1.6f + 4.0f)),
                    juce::Justification::centredLeft, true);
    }

    void drawTickBox (juce::Graphics& g, juce::Component& component, float x, float y, float w, float h,
                      bool ticked, bool isEnabled, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        juce::Rectangle<float> tickBounds (x, y, w, h);

        g.setColour (juce::Colour (0xff111111));
        g.fillRoundedRectangle (tickBounds, 2.0f);

        g.setColour (juce::Colour (0xff444444));
        g.drawRoundedRectangle (tickBounds, 2.0f, 1.0f);

        if (ticked)
        {
            g.setColour (juce::Colour (0xffff6600));
            auto inner = tickBounds.reduced (3.0f);
            g.fillRoundedRectangle (inner, 1.0f);
        }
    }
};
