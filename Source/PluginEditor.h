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
        g.setColour(juce::Colour(0x40000000)); g.fillEllipse(bounds.translated(2.5f,3.5f));
        g.setColour(juce::Colour(0xff0d0f10)); g.fillEllipse(bounds);
        g.setColour(juce::Colour(0xff4f5254)); g.drawEllipse(bounds,2.0f);
        auto ring=bounds.reduced(r*0.08f);
        g.setColour(juce::Colour(0xff282b2d)); g.fillEllipse(ring);
        g.setColour(juce::Colour(0xff686b6c)); g.drawEllipse(ring,1.0f);
        auto face=bounds.reduced(r*0.18f);
        g.setColour(juce::Colour(0xff111314)); g.fillEllipse(face);
        g.setColour(juce::Colour(0xff3f4243)); g.drawEllipse(face,1.0f);
        for(int i=0;i<28;++i){float a=juce::MathConstants<float>::twoPi*(float)i/28.0f;float ri=r*0.78f,ro=r*0.90f;g.setColour(i%4==0?juce::Colour(0xff8f9190):juce::Colour(0xff4d5051));g.drawLine(c.x+std::cos(a)*ri,c.y+std::sin(a)*ri,c.x+std::cos(a)*ro,c.y+std::sin(a)*ro,i%4==0?1.4f:0.8f);}
        float a=rotaryStartAngle+sliderPosProportional*(rotaryEndAngle-rotaryStartAngle);
        float px=c.x+std::cos(a)*(r*0.66f),py=c.y+std::sin(a)*(r*0.66f);
        g.setColour(juce::Colour(0xffffa13a)); g.drawLine(c.x,c.y,px,py,2.6f);
        g.setColour(juce::Colour(0xfff1f1ef)); g.fillEllipse(c.x-2.8f,c.y-2.8f,5.6f,5.6f);
        g.setColour(juce::Colour(0xffbdbab3)); g.fillEllipse(c.x-1.0f,c.y-1.0f,2.0f,2.0f);
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
