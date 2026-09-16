#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ProcessorSetup.inc"
#include "ProcessorDSP.inc"
#include "ProcessorTail.inc"

juce::AudioProcessorValueTreeState::ParameterLayout MaDriveAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Soft Clip Parameters (Default 0 dB / Jam 12)
    params.push_back(std::make_unique<juce::AudioParameterFloat>("input", "Input", juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("threshold", "Threshold", juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("knee", "Knee", juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("clip_output", "Clip Output", juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f));

    // Drive Parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drive", "Drive", juce::NormalisableRange<float>(0.0f, 36.0f, 0.1f), 0.0f)); // Jam 7 (0dB) -> Jam 5 (36dB)
    params.push_back(std::make_unique<juce::AudioParameterFloat>("tone", "Tone", juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));   // Jam 7 (-1) -> Jam 12 (0) -> Jam 5 (+1)
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drive_output", "Drive Output", juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f)); // 0 dB di Jam 12

    // Mix & Character
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mix", "Mix", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 100.0f));         // Jam 7 (0%) -> Jam 5 (100%)
    params.push_back(std::make_unique<juce::AudioParameterFloat>("character", "Character", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 100.0f)); // Jam 7 (0%) -> Jam 5 (100%)

    // Filters
    params.push_back(std::make_unique<juce::AudioParameterFloat>("hpf", "HPF", juce::NormalisableRange<float>(0.0f, 500.0f, 1.0f, 0.4f), 0.0f));       // Jam 7 (0Hz / OFF) -> Jam 5 (500Hz)
    params.push_back(std::make_unique<juce::AudioParameterFloat>("lpf", "LPF", juce::NormalisableRange<float>(3500.0f, 20000.0f, 1.0f, 0.4f), 20000.0f)); // Jam 7 (3500Hz) -> Jam 5 (20000Hz / OFF)

    params.push_back(std::make_unique<juce::AudioParameterFloat>("trim", "Trim", juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f));

    return { params.begin(), params.end() };
}