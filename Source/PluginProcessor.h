#pragma once
#include <JuceHeader.h>

class MaDriveAudioProcessor final : public juce::AudioProcessor
{
public:
    MaDriveAudioProcessor();
    void prepareToPlay (double, int) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout&) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override { return "Ma Drive"; }
    bool acceptsMidi() const override { return false; } bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; } double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; } int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {} const juce::String getProgramName (int) override { return "Default"; }
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;
    juce::AudioProcessorValueTreeState parameters;
    std::atomic<float> inputPeak { -100.0f }, outputPeak { -100.0f }, reduction { 0.0f };
    std::array<std::atomic<float>, 3> monitorPeaks { -100.0f, -100.0f, -100.0f };
    std::atomic<float> thresholdForDisplay { -12.0f };
    std::array<float, 256> inputHistory {}, outputHistory {}, grHistory {};
    std::atomic<int> historyWrite { 0 };
    void copyAB (bool toB); void recallAB (bool useB); void switchAB (bool useB);
private:
    static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout();
    float shape (float x, int clipStyle, int driveMode, float character) const;
    std::array<std::unique_ptr<juce::dsp::Oversampling<float>>, 4> oversampling;
    juce::dsp::IIR::Filter<float> hpfL, hpfR, lpfL, lpfR, bandLowL, bandLowR, bandHighL, bandHighR;
    juce::AudioBuffer<float> dryBuffer;
    juce::ValueTree stateA, stateB;
    bool activeStateB = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MaDriveAudioProcessor)
};
