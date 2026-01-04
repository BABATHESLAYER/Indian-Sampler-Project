#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class GlobalLED : public juce::Component, public juce::Timer
{
public:
    GlobalLED()
    {
        setOpaque(false);
    }

    void triggerFlash()
    {
        isActive = true;
        alpha = 1.0f;
        startTimer(30); // update loop
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(4.0f);

        // Glow effect
        if (isActive)
        {
            juce::Colour glowCol = juce::Colours::orange.withAlpha(alpha);
            g.setColour(glowCol);
            g.fillEllipse(bounds.expanded(2.0f));
        }

        // The Physical LED Body
        g.setColour(isActive ? juce::Colours::red : juce::Colours::darkred.darker());
        g.fillEllipse(bounds);

        // Reflection
        g.setColour(juce::Colours::white.withAlpha(0.4f));
        g.fillEllipse(bounds.reduced(bounds.getWidth() * 0.3f).translated(-bounds.getWidth()*0.1f, -bounds.getHeight()*0.1f));
    }

    void timerCallback() override
    {
        alpha -= 0.1f;
        if (alpha <= 0.0f)
        {
            isActive = false;
            stopTimer();
        }
        repaint();
    }

private:
    bool isActive = false;
    float alpha = 0.0f;
};
