/*
  ==============================================================================

    LookAndFeel.cpp
    Created: 16 Mar 2024 3:09:57pm
    Author:  Simon Perrier

  ==============================================================================
*/

#include "LookAndFeel.h"

LookAndFeel::LookAndFeel()
{
    // JetBrains Mono font
    // LRN construct new juce::Typeface object from the pointer to the font data
    juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::JetBrainsMonoRegular_ttf, BinaryData::JetBrainsMonoRegular_ttfSize);
    setDefaultSansSerifTypeface(typeface);
    
    // Default accent color
    // TODO: override for specific parts of synth maybe
    auto accentColor = juce::Colour(220, 220, 220);
    
    // Plugin background
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(35, 35, 35));
    
    // Rotary slider
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0, 0, 0));
    setColour(juce::Slider::rotarySliderFillColourId, accentColor);
    setColour(juce::Slider::thumbColourId, accentColor);
    
    // Text button
    setColour(juce::TextButton::buttonColourId, juce::Colour(0, 0, 0));
    setColour(juce::TextButton::buttonOnColourId, accentColor);
    setColour(juce::TextButton::textColourOffId, accentColor);
    setColour(juce::TextButton::textColourOnId, juce::Colour(0, 0, 0));
    setColour(juce::ComboBox::outlineColourId, juce::Colour(0, 0, 0));
}

void LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int /*height*/, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    auto outlineColor = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
    auto fillColor = slider.findColour(juce::Slider::rotarySliderFillColourId);
    auto dialColor = slider.findColour(juce::Slider::thumbColourId);

    auto bounds = juce::Rectangle<int>(x, y, width, width).toFloat()
                              .withTrimmedLeft(16.0f).withTrimmedRight(16.0f)
                              .withTrimmedTop(0.0f).withTrimmedBottom(8.0f);

    auto radius = bounds.getWidth() / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = 6.0f;
    auto arcRadius = radius - lineW / 2.0f;

    auto arg = toAngle - juce::MathConstants<float>::halfPi;
    auto dialW = 3.0f;
    auto dialRadius = arcRadius - 6.0f;

    auto center = bounds.getCentre();
    auto strokeType = juce::PathStrokeType(lineW, juce::PathStrokeType::curved,
                                           juce::PathStrokeType::butt);

    juce::Path backgroundArc;
    backgroundArc.addCentredArc(center.x, center.y, arcRadius, arcRadius, 0.0f,
                                rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(outlineColor);
    g.strokePath(backgroundArc, strokeType);

    if (slider.isEnabled()) {
        juce::Path valueArc;
        valueArc.addCentredArc(center.x, center.y, arcRadius, arcRadius, 0.0f,
                               rotaryStartAngle, toAngle, true);
        g.setColour(fillColor);
        g.strokePath(valueArc, strokeType);
    }

    juce::Point<float> thumbPoint(center.x + dialRadius * std::cos(arg),
                                  center.y + dialRadius * std::sin(arg));
    g.setColour(dialColor);
    g.drawLine(center.x, center.y, thumbPoint.x, thumbPoint.y, dialW);
    g.fillEllipse(juce::Rectangle<float>(dialW, dialW).withCentre(thumbPoint));
    g.fillEllipse(juce::Rectangle<float>(dialW, dialW).withCentre(center));
}

