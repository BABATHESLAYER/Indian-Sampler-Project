#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class DesiLookAndFeel : public juce::LookAndFeel_V4
{
public:
    DesiLookAndFeel()
    {
        // Define Rajasthani Palette
        static const juce::Colour darkWood = juce::Colour(0xFF3E2723);
        static const juce::Colour brightGold = juce::Colour(0xFFFFD700);
        static const juce::Colour deepRed = juce::Colour(0xFF880E4F);

        setColour(juce::ResizableWindow::backgroundColourId, darkWood);
        setColour(juce::Slider::thumbColourId, brightGold);
        setColour(juce::Slider::trackColourId, deepRed);
        setColour(juce::TextButton::buttonColourId, darkWood.brighter(0.1f));
        setColour(juce::TextButton::textColourOffId, brightGold);
        setColour(juce::ListBox::backgroundColourId, darkWood.darker(0.3f));
        setColour(juce::ListBox::textColourId, brightGold);
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override
    {
        auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
        auto centreX = (float) x + (float) width  * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // Gold Ring
        g.setColour (juce::Colours::gold);
        g.drawEllipse (rx, ry, rw, rw, 2.0f);

        // Inner Wood Circle
        g.setColour (findColour(juce::ResizableWindow::backgroundColourId).darker(0.2f));
        g.fillEllipse (rx + 2, ry + 2, rw - 4, rw - 4);

        // Pointer (Vector style)
        juce::Path p;
        auto pointerLength = radius * 0.8f;
        auto pointerThickness = 3.0f;
        p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));

        g.setColour (juce::Colours::gold);
        g.fillPath (p);
    }

    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto cornerSize = 6.0f;
        auto bounds = button.getLocalBounds().toFloat().reduced (0.5f, 0.5f);

        auto baseColour = backgroundColour.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                                          .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);

        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            baseColour = baseColour.contrasting (shouldDrawButtonAsDown ? 0.2f : 0.05f);

        g.setColour (baseColour);
        g.fillRoundedRectangle (bounds, cornerSize);

        g.setColour (juce::Colours::gold);
        g.drawRoundedRectangle (bounds, cornerSize, 1.5f);
    }
};
