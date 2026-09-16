cmake_minimum_required(VERSION 3.22)
project(MaDrive VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_subdirectory(external/JUCE)

juce_add_plugin(MaDrive
    COMPANY_NAME "WADIDAW"
    PRODUCT_NAME "Ma Drive"
    BUNDLE_ID "com.wadidaw.madrive"
    PLUGIN_MANUFACTURER_CODE Wadi
    PLUGIN_CODE MDrv
    FORMATS VST3 Standalone
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)

juce_generate_juce_header(MaDrive)
target_sources(MaDrive PRIVATE
    Source/PluginProcessor.cpp Source/PluginProcessor.h
    Source/PluginEditor.cpp Source/PluginEditor.h)
target_include_directories(MaDrive PRIVATE Source)
target_compile_definitions(MaDrive PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(MaDrive PRIVATE juce::juce_audio_utils juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags)
