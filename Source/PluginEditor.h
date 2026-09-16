#pragma once
#include "PluginProcessor.h"

class MaDriveHardwareLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override
    {
        juce::ignoreUnused(slider);
        auto bounds = juce::Rectangle<float>((float)x, (float)y, (float)width, (float)height).reduced(5.0f);
        auto centre = bounds.getCentre();
        const float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
        g.setColour(juce::Colour(0x55000000)); g.fillEllipse(bounds.translated(3.0f, 4.0f));
        g.setColour(juce::Colour(0xff171919)); g.fillEllipse(bounds);
        g.setColour(juce::Colour(0xff626565)); g.drawEllipse(bounds, 1.6f);
        auto inner = bounds.reduced(radius * 0.10f);
        g.setColour(juce::Colour(0xff2c2f30)); g.fillEllipse(inner);
        g.setColour(juce::Colour(0xff777979)); g.drawEllipse(inner, 1.0f);
        for (int i = 0; i < 28; ++i)
        {
            const float a = juce::MathConstants<float>::twoPi * (float)i / 28.0f;
            const float ri = radius * 0.76f, ro = radius * 0.88f;
            g.setColour(i % 4 == 0 ? juce::Colour(0xffa5a5a0) : juce::Colour(0xff565959));
            g.drawLine(centre.x + std::cos(a) * ri, centre.y + std::sin(a) * ri,
                       centre.x + std::cos(a) * ro, centre.y + std::sin(a) * ro,
                       i % 4 == 0 ? 1.4f : 0.75f);
        }
        const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        const float px = centre.x + std::cos(angle) * radius * 0.67f;
        const float py = centre.y + std::sin(angle) * radius * 0.67f;
        g.setColour(juce::Colour(0xffff9b35)); g.drawLine(centre.x, centre.y, px, py, 2.6f);
        g.setColour(juce::Colour(0xffeeeeea)); g.fillEllipse(centre.x - 3.0f, centre.y - 3.0f, 6.0f, 6.0f);
        g.setColour(juce::Colour(0xffff9b35)); g.fillEllipse(centre.x - 1.0f, centre.y - 1.0f, 2.0f, 2.0f);
    }
};

class MaDriveAudioProcessorEditor final : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit MaDriveAudioProcessorEditor(MaDriveAudioProcessor&);
    ~MaDriveAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override { repaint(); }
    MaDriveAudioProcessor& p;
    MaDriveHardwareLookAndFeel hardwareLookAndFeel;
    juce::OwnedArray<juce::Slider> knobs;
    juce::OwnedArray<juce::Label> labels;
    juce::OwnedArray<juce::TextButton> buttons;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> buttonAttachments;
    bool activeB = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MaDriveAudioProcessorEditor)
};
