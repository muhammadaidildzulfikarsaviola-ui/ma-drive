#pragma once
#include <JuceHeader.h>

class MaDriveAudioProcessor final : public juce::AudioProcessor
{
public:
    MaDriveAudioProcessor();
    ~MaDriveAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override { return "Ma Drive"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Deklarasi tanpa inline body agar tidak bentrok dengan PluginProcessor.cpp / ProcessorSetup.inc
    static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout();
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // APVTS & Atomic Variables untuk Metering UI
    juce::AudioProcessorValueTreeState parameters;
    std::atomic<float> inputPeak { -100.0f }, outputPeak { -100.0f }, reduction { 0.0f }, softClipGR { 0.0f };
    std::array<std::atomic<float>, 3> monitorPeaks { -100.0f, -100.0f, -100.0f };
    std::array<std::atomic<float>, 4> rackPeaks { -100.0f, -100.0f, -100.0f, -100.0f };
    std::atomic<float> thresholdForDisplay { -8.0f };

    // Waveform & History Buffers
    std::array<float, 256> inputHistory {}, outputHistory {}, grHistory {};
    std::atomic<int> historyWrite { 0 };

    // Utility Method A/B State
    void copyAB(bool toB);
    void recallAB(bool useB);
    void switchAB(bool useB) { recallAB(useB); }

private:
    float shape(float x, int mode, int type, float knee) const;

    // DSP & Oversampling Components
    std::array<std::unique_ptr<juce::dsp::Oversampling<float>>, 4> oversampling;
    juce::dsp::IIR::Filter<float> hpfL, hpfR, lpfL, lpfR, bandLowL, bandLowR, bandHighL, bandHighR;
    juce::AudioBuffer<float> dryBuffer;

    // State A/B Trees
    juce::ValueTree stateA, stateB;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MaDriveAudioProcessor)
};