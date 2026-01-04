#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>

class SampleBrowser : public juce::Component, public juce::ListBoxModel, public juce::Button::Listener
{
public:
    std::function<void(juce::File, int, bool)> onLoadSample;

    SampleBrowser()
    {
        addAndMakeVisible(listBox);
        listBox.setModel(this);
        listBox.setRowHeight(30);

        addAndMakeVisible(loadButton);
        loadButton.setButtonText("Load Sample");
        loadButton.addListener(this);

        // Setup Transport for Preview
        // formatManager.registerBasicFormats(); // moved to Processor

        addAndMakeVisible(modeToggle);
        modeToggle.setButtonText("Melodic Mode (Stretch)");
        modeToggle.setToggleState(true, juce::dontSendNotification);

        addAndMakeVisible(octaveSelector);
        for (int i = 1; i <= 8; ++i)
            octaveSelector.addItem("Octave " + juce::String(i), i);
        octaveSelector.setSelectedId(4); // Default C4

        refreshList();
    }

    void refreshList()
    {
        files.clear();
        // Look in Samples directory relative to executable or project root
        juce::File samplesDir = juce::File::getCurrentWorkingDirectory().getChildFile("Samples");

        // Recursively find audio files
        juce::Array<juce::File> results;
        samplesDir.findChildFiles(results, juce::File::findFiles, true, "*.wav;*.mp3;*.aiff");

        for (auto f : results)
            files.add(f);

        listBox.updateContent();
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(10);
        auto topBar = area.removeFromTop(40);

        octaveSelector.setBounds(topBar.removeFromLeft(100));
        topBar.removeFromLeft(10);
        modeToggle.setBounds(topBar.removeFromLeft(150));
        topBar.removeFromLeft(10);
        loadButton.setBounds(topBar);

        area.removeFromTop(10);
        listBox.setBounds(area);
    }

    int getNumRows() override { return files.size(); }

    void buttonClicked (juce::Button* b) override
    {
        if (b == &loadButton)
        {
            int row = listBox.getSelectedRow();
            if (row >= 0 && row < files.size())
            {
                if (onLoadSample)
                {
                    onLoadSample(files[row], octaveSelector.getSelectedId(), modeToggle.getToggleState());
                }
            }
        }
    }

    void listBoxItemClicked(int row, const juce::MouseEvent&) override
    {
        // Optional: Preview functionality could go here
    }

    void paintListBoxItem (int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override
    {
        if (rowIsSelected)
            g.fillAll (juce::Colours::gold.withAlpha (0.4f));

        g.setColour (juce::Colours::white);
        g.setFont (14.0f);

        if (juce::isPositiveAndBelow(rowNumber, files.size()))
        {
            auto f = files[rowNumber];
            juce::File root = juce::File::getCurrentWorkingDirectory().getChildFile("Samples");

            // Draw Filename
            g.drawText (f.getRelativePathFrom(root), 30, 0, width - 30, height, juce::Justification::centredLeft, true);

            // Draw "Play" Icon on the left
            g.setColour(juce::Colours::lightgreen);
            juce::Path playArrow;
            playArrow.addTriangle(5.0f, 5.0f, 5.0f, (float)height - 5.0f, 20.0f, (float)height / 2.0f);
            g.fillPath(playArrow);
        }
    }

    void listBoxItemDoubleClicked(int row, const juce::MouseEvent&) override
    {
        // Preview on double click
        if (row >= 0 && row < files.size())
        {
            playPreview(files[row]);
        }
    }

    // Explicit Play method
    void playPreview(juce::File file)
    {
        if (onPreviewSample)
            onPreviewSample(file);
    }

    std::function<void(juce::File)> onPreviewSample;

private:
    juce::ListBox listBox;
    juce::TextButton loadButton;
    juce::ToggleButton modeToggle;
    juce::ComboBox octaveSelector;
    juce::Array<juce::File> files;
};
