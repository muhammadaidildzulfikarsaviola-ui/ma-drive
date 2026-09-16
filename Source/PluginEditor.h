#pragma once
#include "PluginProcessor.h"
class MaDriveHardwareLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g,int x,int y,int width,int height,float sliderPosProportional,float rotaryStartAngle,float rotaryEndAngle,juce::Slider&) override
    {
        auto bounds=juce::Rectangle<float>((float)x,(float)y,(float)width,(float)height).reduced(5.0f);
        auto c=bounds.getCentre();
        float r=juce::jmin(bounds.getWidth(),bounds.getHeight())*0.5f;
        g.setColour(juce::Colour(0x30000000));g.fillEllipse(bounds.translated(2.0f,3.0f));
        g.setColour(juce::Colour(0xff17191a));g.fillEllipse(bounds);
        g.setColour(juce::Colour(0xff303334));g.drawEllipse(bounds,2.0f);
        auto inner=bounds.reduced(r*0.12f);
        g.setColour(juce::Colour(0xff090a0b));g.fillEllipse(inner);
        g.setColour(juce::Colour(0xff55585a));g.drawEllipse(inner,1.0f);
        g.setColour(juce::Colour(0xff777b7d));
        for(int i=0;i<12;++i){float a=juce::MathConstants<float>::twoPi*(float)i/12.0f;float ri=r*0.78f,ro=r*0.91f;g.drawLine(c.x+std::cos(a)*ri,c.y+std::sin(a)*ri,c.x+std::cos(a)*ro,c.y+std::sin(a)*ro,1.0f);}
        float a=rotaryStartAngle+sliderPosProportional*(rotaryEndAngle-rotaryStartAngle);
        float px=c.x+std::cos(a)*(r*0.68f),py=c.y+std::sin(a)*(r*0.68f);
        g.setColour(juce::Colour(0xffffa13a));g.drawLine(c.x,c.y,px,py,2.2f);
        g.setColour(juce::Colour(0xffeeeeee));g.fillEllipse(c.x-2.5f,c.y-2.5f,5.0f,5.0f);
        g.setColour(juce::Colour(0xffffa13a));g.fillEllipse(c.x-1.2f,c.y-1.2f,2.4f,2.4f);
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
    void timerCallback() override { p.copyAB(activeB); repaint(); }
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
