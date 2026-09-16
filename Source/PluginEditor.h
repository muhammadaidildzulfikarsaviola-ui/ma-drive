#pragma once
#include "PluginProcessor.h"
class MaDriveAudioProcessorEditor final : public juce::AudioProcessorEditor, private juce::Timer
{
public: explicit MaDriveAudioProcessorEditor(MaDriveAudioProcessor&); ~MaDriveAudioProcessorEditor() override; void paint(juce::Graphics&) override; void resized() override;
private: void timerCallback() override { repaint(); } MaDriveAudioProcessor& p; juce::OwnedArray<juce::Slider> knobs; juce::OwnedArray<juce::Label> labels; juce::OwnedArray<juce::TextButton> buttons; std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments; std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> buttonAttachments; JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MaDriveAudioProcessorEditor)
};
