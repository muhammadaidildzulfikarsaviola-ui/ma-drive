#pragma once
#include "PluginProcessor.h"

class MaDriveHardwareLookAndFeel : public juce::LookAndFeel_V4
{
public:
    // 1. Hardware Knob Renderer (Bulat Proporsional + Cap Metal)
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override
    {
        juce::ignoreUnused(slider);
        auto bounds = juce::Rectangle<float>((float)x, (float)y, (float)width, (float)height).reduced(2.0f);
        float diameter = juce::jmin(bounds.getWidth(), bounds.getHeight());
        auto knobRect = juce::Rectangle<float>(bounds.getCentreX() - diameter * 0.5f, bounds.getCentreY() - diameter * 0.5f, diameter, diameter);
        auto centre = knobRect.getCentre();
        float radius = diameter * 0.5f;

        // Shadow & Outer Metal Bezel
        g.setColour(juce::Colour(0x66000000));
        g.fillEllipse(knobRect.translated(0.0f, 3.0f));

        g.setColour(juce::Colour(0xff1f2123));
        g.fillEllipse(knobRect);
        g.setColour(juce::Colour(0xff525558));
        g.drawEllipse(knobRect, 1.5f);

        // Inner Metal Cap
        auto inner = knobRect.reduced(radius * 0.18f);
        juce::ColourGradient capGrad(juce::Colour(0xff3a3d40), inner.getX(), inner.getY(),
                                     juce::Colour(0xff18191a), inner.getRight(), inner.getBottom(), false);
        g.setGradientFill(capGrad);
        g.fillEllipse(inner);
        g.setColour(juce::Colour(0xff686b6e));
        g.drawEllipse(inner, 1.0f);

        // Indicator Line
        const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        const float px = centre.x + std::sin(angle) * (radius * 0.78f);
        const float py = centre.y - std::cos(angle) * (radius * 0.78f);

        g.setColour(juce::Colour(0xffff8a2a));
        g.drawLine(centre.x, centre.y, px, py, 2.5f);
        g.setColour(juce::Colours::white);
        g.fillEllipse(centre.x - 2.0f, centre.y - 2.0f, 4.0f, 4.0f);
    }

    // 2. Hardware Toggle / Rocker Switch Renderer
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                              bool isMouseOverButton, bool isButtonDown) override
    {
        juce::ignoreUnused(backgroundColour, isMouseOverButton, isButtonDown);
        auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
        bool isOn = button.getToggleState() || button.getState() == juce::Button::buttonDown;

        // Base Switch Frame
        g.setColour(juce::Colour(0xff121314));
        g.fillRoundedRectangle(bounds, 3.0f);
        g.setColour(juce::Colour(0xff44474a));
        g.drawRoundedRectangle(bounds, 3.0f, 1.0f);

        // Switch Cap (On/Off Color)
        auto cap = bounds.reduced(2.0f);
        if (isOn)
        {
            juce::ColourGradient onGrad(juce::Colour(0xffff9d42), cap.getX(), cap.getY(),
                                        juce::Colour(0xffd66200), cap.getX(), cap.getBottom(), false);
            g.setGradientFill(onGrad);
        }
        else
        {
            juce::ColourGradient offGrad(juce::Colour(0xff2d3033), cap.getX(), cap.getY(),
                                         juce::Colour(0xff1a1b1c), cap.getX(), cap.getBottom(), false);
            g.setGradientFill(offGrad);
        }
        g.fillRoundedRectangle(cap, 2.0f);
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