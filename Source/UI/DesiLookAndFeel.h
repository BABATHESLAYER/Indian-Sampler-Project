#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class DesiLookAndFeel : public juce::LookAndFeel_V4
{
public:
    DesiLookAndFeel()
    {
        // "Cyber-Desi" Palette
        static const juce::Colour darkGrey = juce::Colour(0xFF1E1E1E);
        static const juce::Colour darkerGrey = juce::Colour(0xFF121212);
        static const juce::Colour neonGold = juce::Colour(0xFFFFD700);
        static const juce::Colour cyberBlue = juce::Colour(0xFF00E5FF);
        static const juce::Colour lightText = juce::Colour(0xFFEEEEEE);

        // General
        setColour(juce::ResizableWindow::backgroundColourId, darkGrey);
        setColour(juce::Label::textColourId, lightText);
        setColour(juce::GroupComponent::textColourId, neonGold);
        setColour(juce::GroupComponent::outlineColourId, neonGold.withAlpha(0.6f));

        // Buttons
        setColour(juce::TextButton::buttonColourId, darkerGrey);
        setColour(juce::TextButton::buttonOnColourId, neonGold.withAlpha(0.2f));
        setColour(juce::TextButton::textColourOffId, neonGold);
        setColour(juce::TextButton::textColourOnId, juce::Colours::white);

        // ListBox
        setColour(juce::ListBox::backgroundColourId, darkerGrey);
        setColour(juce::ListBox::outlineColourId, neonGold.withAlpha(0.3f));
        setColour(juce::ListBox::textColourId, lightText);

        // Sliders
        setColour(juce::Slider::thumbColourId, neonGold);
        setColour(juce::Slider::rotarySliderFillColourId, cyberBlue);
        setColour(juce::Slider::rotarySliderOutlineColourId, darkerGrey.brighter(0.1f));

        // TabbedComponent
        setColour(juce::TabbedComponent::backgroundColourId, darkGrey);
        setColour(juce::TabbedComponent::outlineColourId, neonGold);
        setColour(juce::TabbedButtonBar::frontTextColourId, neonGold);
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

        // Dark track
        g.setColour (juce::Colours::black);
        g.drawEllipse (rx, ry, rw, rw, 4.0f);

        // Active Arc (Cyber Blue)
        juce::Path p;
        p.addArc (rx, ry, rw, rw, rotaryStartAngle, angle, true);
        juce::PathStrokeType (4.0f).createStrokedPath (p, p);
        g.setColour (findColour(juce::Slider::rotarySliderFillColourId));
        g.fillPath (p);

        // Neon Gold Indicator
        juce::Path pointer;
        auto pointerLength = radius * 0.9f;
        auto pointerThickness = 3.0f;
        pointer.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        pointer.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));
        g.setColour (findColour(juce::Slider::thumbColourId));
        g.fillPath (pointer);
    }

    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto cornerSize = 4.0f; // Sharp corners
        auto bounds = button.getLocalBounds().toFloat().reduced (0.5f, 0.5f);

        auto baseColour = backgroundColour;
        if (shouldDrawButtonAsDown)
            baseColour = baseColour.brighter(0.2f);
        else if (shouldDrawButtonAsHighlighted)
            baseColour = baseColour.brighter(0.1f);

        g.setColour (baseColour);
        g.fillRoundedRectangle (bounds, cornerSize);

        // Glowing Border
        g.setColour (findColour(juce::TextButton::textColourOffId).withAlpha(0.6f));
        g.drawRoundedRectangle (bounds, cornerSize, 1.5f);
    }

    // Custom Tab Look
    void drawTabButton (juce::TabBarButton& button, juce::Graphics& g, bool isMouseOver, bool isMouseDown) override
    {
        const auto activeArea = button.getActiveArea();
        auto b = button.getLocalBounds().toFloat();

        // Background
        g.setColour(button.getTabbedButtonBar().getTabBackgroundColour(button.getIndex()));
        g.fillRect(b);

        // Border
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.drawRect(b, 1.0f);

        // Active Indicator (Neon Gold underline)
        if (button.isFrontTab())
        {
            g.setColour(findColour(juce::TabbedComponent::outlineColourId));
            g.fillRect(b.removeFromBottom(3.0f));
        }

        // Text
        g.setColour(button.isFrontTab() ? findColour(juce::TabbedButtonBar::frontTextColourId)
                                        : findColour(juce::TabbedButtonBar::tabTextColourId));
        g.setFont(16.0f);
        g.drawText(button.getButtonText(), activeArea, juce::Justification::centred, true);
    }
};
