#pragma once
#include "PluginProcessor.h"
class MaDriveHardwareLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g,int x,int y,int width,int height,float sliderPosProportional,float rotaryStartAngle,float rotaryEndAngle,juce::Slider& slider) override
    {
        auto bounds=juce::Rectangle<float>((float)x,(float)y,(float)width,(float)height).reduced(5.0f);
        auto c=bounds.getCentre();
        float r=juce::jmin(bounds.getWidth(),bounds.getHeight())*0.5f;
        g.setColour(juce::Colour(0x45000000)); g.fillEllipse(bounds.translated(3.0f,4.0f));
        g.setColour(juce::Colour(0xff171919)); g.fillEllipse(bounds);
        g.setColour(juce::Colour(0xff5a5d5d)); g.drawEllipse(bounds,2.0f);
        auto ring=bounds.reduced(r*0.08f); g.setColour(juce::Colour(0xff2c2f30)); g.fillEllipse(ring); g.setColour(juce::Colour(0xff747777)); g.drawEllipse(ring,1.2f);
        auto face=bounds.reduced(r*0.19f); g.setColour(juce::Colour(0xff101213)); g.fillEllipse(face); g.setColour(juce::Colour(0xff424546)); g.drawEllipse(face,1.0f);
        for(int i=0;i<28;++i){float a=juce::MathConstants<float>::twoPi*(float)i/28.0f;float ri=r*0.79f,ro=r*0.90f;g.setColour(i%4==0?juce::Colour(0xff9a9a97):juce::Colour(0xff555859));g.drawLine(c.x+std::cos(a)*ri,c.y+std::sin(a)*ri,c.x+std::cos(a)*ro,c.y+std::sin(a)*ro,i%4==0?1.5f:0.8f);}
        float a=rotaryStartAngle+sliderPosProportional*(rotaryEndAngle-rotaryStartAngle);
        float px=c.x+std::cos(a)*(r*0.67f),py=c.y+std::sin(a)*(r*0.67f);
        g.setColour(juce::Colour(0xffffa13a)); g.drawLine(c.x,c.y,px,py,2.6f);
        g.setColour(juce::Colour(0xffefefec)); g.fillEllipse(c.x-3,c.y-3,6,6);
        g.setColour(juce::Colour(0xffffa13a)); g.fillEllipse(c.x-1,c.y-1,2,2);
        juce::ignoreUnused(slider);
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
    bool activeB=false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MaDriveAudioProcessorEditor)
};
