#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
StereoReverbAudioProcessorEditor::StereoReverbAudioProcessorEditor (StereoReverbAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p),
      roomSizeAttachment(processorRef.apvts, "roomSize", roomSizeSlider),
      decayTimeAttachment(processorRef.apvts, "decayTime", decayTimeSlider)
{
    setSize (380, 220);

    // Room Size Slider
    roomSizeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    roomSizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    roomSizeSlider.setRange(0.0, 1.0, 0.001);
    roomSizeSlider.setSkewFactorFromMidPoint(0.5);
    roomSizeSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::deepskyblue);
    roomSizeSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    roomSizeSlider.setTooltip("Room Size (0 = Small, 1 = Large)");
    addAndMakeVisible(roomSizeSlider);

    roomSizeLabel.setText("Room Size", juce::dontSendNotification);
    roomSizeLabel.setJustificationType(juce::Justification::centred);
    roomSizeLabel.attachToComponent(&roomSizeSlider, false);
    addAndMakeVisible(roomSizeLabel);

    // Decay Time Slider
    decayTimeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    decayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    decayTimeSlider.setRange(0.1, 10.0, 0.01);
    decayTimeSlider.setSkewFactorFromMidPoint(1.0);
    decayTimeSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::orange);
    decayTimeSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    decayTimeSlider.setTooltip("Decay Time (seconds)");
    addAndMakeVisible(decayTimeSlider);

    decayTimeLabel.setText("Decay Time", juce::dontSendNotification);
    decayTimeLabel.setJustificationType(juce::Justification::centred);
    decayTimeLabel.attachToComponent(&decayTimeSlider, false);
    addAndMakeVisible(decayTimeLabel);
}

StereoReverbAudioProcessorEditor::~StereoReverbAudioProcessorEditor()
{
}

void StereoReverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour(0xff151c1e));

    auto area = getLocalBounds().reduced(10);
    g.setColour(juce::Colours::white.withAlpha(0.08f));
    g.fillRoundedRectangle(area.toFloat(), 16.0f);

    g.setColour(juce::Colours::white);
    g.setFont(22.0f);
    g.drawFittedText("Stereo Reverb", area.removeFromTop(36), juce::Justification::centred, 1);
}

void StereoReverbAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(30);

    auto top = area.removeFromTop(60);
    area.removeFromTop(10);

    auto sliderWidth = 140;
    auto sliderHeight = 120;

    roomSizeSlider.setBounds(area.removeFromLeft(sliderWidth).withSizeKeepingCentre(sliderWidth, sliderHeight));
    area.removeFromLeft(20);
    decayTimeSlider.setBounds(area.removeFromLeft(sliderWidth).withSizeKeepingCentre(sliderWidth, sliderHeight));
}
```

```cmake
#