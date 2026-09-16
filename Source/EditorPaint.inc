#include "PluginProcessor.h"
#include "PluginEditor.h"

MaDriveAudioProcessorEditor::MaDriveAudioProcessorEditor(MaDriveAudioProcessor& p_)
    : AudioProcessorEditor(&p_), p(p_)
{
    // --- LAMBDA HELPER SET ANGLE KNOB BERDASARKAN POSISI JAM ANALOG ---
    auto setKnobAngles = [](juce::Slider* slider, float startClockHour, float endClockHour) {
        float startAngle = juce::MathConstants<float>::pi * ((startClockHour - 12.0f) / 6.0f);
        float endAngle   = juce::MathConstants<float>::pi * ((endClockHour - 12.0f) / 6.0f);
        slider->setRotaryParameters(startAngle, endAngle, true);
    };

    // --- INISIALISASI 12 KNOBS ---
    for (int i = 0; i < 12; ++i)
    {
        auto* s = knobs.add(new juce::Slider());
        s->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(s);

        auto* l = labels.add(new juce::Label());
        l->setFont(juce::Font(11.0f, juce::Font::bold));
        l->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(l);
    }

    // Set Rentang Jam
    for (int i = 0; i <= 3; ++i) setKnobAngles(knobs[i], 7.0f, 5.0f); // Soft Clip: Default Jam 12
    setKnobAngles(knobs[4], 7.0f, 5.0f);  // Drive
    setKnobAngles(knobs[5], 7.0f, 5.0f);  // Tone
    setKnobAngles(knobs[6], 7.0f, 5.0f);  // Output Drive
    setKnobAngles(knobs[7], 7.0f, 5.0f);  // Mix
    setKnobAngles(knobs[8], 7.0f, 5.0f);  // Character
    setKnobAngles(knobs[9], 7.0f, 5.0f);  // HPF
    setKnobAngles(knobs[10], 7.0f, 5.0f); // LPF
    setKnobAngles(knobs[11], 7.0f, 5.0f); // Trim

    // Label Knobs
    const char* knobNames[] = {
        "INPUT", "THRESHOLD", "KNEE", "OUTPUT",
        "DRIVE", "TONE", "OUTPUT",
        "MIX", "CHARACTER", "HPF", "LPF", "TRIM"
    };
    for (int i = 0; i < 12; ++i) labels[i]->setText(knobNames[i], juce::dontSendNotification);

    // --- INISIALISASI 17 BUTTONS / SWITCHES ---
    const char* btnLabels[] = {
        "SOFT", "MEDIUM", "HARD",          // 0-2
        "1x", "2x", "4x", "8x",            // 3-6
        "STEREO", "M/S", "MULTI BAND",     // 7-9
        "GROWL", "ODD", "TAPE",            // 10-12
        "BYPASS", "A", "B", "SUPPORT ME"  // 13-16
    };

    for (int i = 0; i < 17; ++i)
    {
        auto* b = buttons.add(new juce::TextButton(btnLabels[i]));
        b->setClickingTogglesState(true);
        addAndMakeVisible(b);
    }

    // Radio Groups (Saklar Pilih Salah Satu)
    for (int i = 0; i < 3; ++i)   buttons[i]->setRadioGroupId(1001);
    for (int i = 3; i < 7; ++i)   buttons[i]->setRadioGroupId(1002);
    for (int i = 7; i < 10; ++i)  buttons[i]->setRadioGroupId(1003);
    for (int i = 10; i < 13; ++i) buttons[i]->setRadioGroupId(1004);

    buttons[0]->setToggleState(true, juce::dontSendNotification);
    buttons[3]->setToggleState(true, juce::dontSendNotification);
    buttons[7]->setToggleState(true, juce::dontSendNotification);
    buttons[10]->setToggleState(true, juce::dontSendNotification);

    setSize(900, 580);
}

MaDriveAudioProcessorEditor::~MaDriveAudioProcessorEditor()
{
}

// Import logika gambar VU Meter & Canvas Paint
#include "EditorPaint.inc"

// --- IMPLEMENTASI RESIZED (LAYOUT POSISI KOMPONEN) ---
void MaDriveAudioProcessorEditor::resized()
{
    auto boundsArea = getLocalBounds().reduced(10, 2);
    
    // Header Buttons (A, B, Options, Settings)
    buttons[14]->setBounds(16, 12, 35, 22); // A
    buttons[15]->setBounds(55, 12, 35, 22); // B

    boundsArea.removeFromTop(45);
    auto footerArea = boundsArea.removeFromBottom(45);

    // Footer Layout
    buttons[16]->setBounds(footerArea.getCentreX() - 60, footerArea.getY() + 8, 120, 26); // SUPPORT ME
    buttons[13]->setBounds(footerArea.getRight() - 170, footerArea.getY() + 8, 90, 26);    // BYPASS
    
    knobs[11]->setBounds(footerArea.getRight() - 60, footerArea.getY() + 2, 32, 32);     // Trim
    labels[11]->setBounds(footerArea.getRight() - 70, footerArea.getY() + 32, 52, 12);

    int panelWidth = boundsArea.getWidth() / 3;
    auto leftArea   = boundsArea.removeFromLeft(panelWidth).reduced(2);
    auto rightArea  = boundsArea.removeFromRight(panelWidth).reduced(2);

    // --- PANEL KIRI (SOFT CLIP) ---
    int leftY = leftArea.getY() + 145; // Setelah area VU Meter
    
    // Knob Input & Threshold
    labels[0]->setBounds(leftArea.getX() + 25, leftY, 70, 14);
    knobs[0]->setBounds(leftArea.getX() + 35, leftY + 16, 50, 50);

    labels[1]->setBounds(leftArea.getX() + 145, leftY, 80, 14);
    knobs[1]->setBounds(leftArea.getX() + 160, leftY + 16, 50, 50);

    // Knob Knee & Output Soft Clip
    labels[2]->setBounds(leftArea.getX() + 25, leftY + 80, 70, 14);
    knobs[2]->setBounds(leftArea.getX() + 35, leftY + 96, 50, 50);

    labels[3]->setBounds(leftArea.getX() + 145, leftY + 80, 80, 14);
    knobs[3]->setBounds(leftArea.getX() + 160, leftY + 96, 50, 50);

    // Switches Clip Style (Soft, Medium, Hard)
    int btnY1 = leftY + 165;
    buttons[0]->setBounds(leftArea.getX() + 10, btnY1, 80, 22);
    buttons[1]->setBounds(leftArea.getX() + 96, btnY1, 80, 22);
    buttons[2]->setBounds(leftArea.getX() + 182, btnY1, 80, 22);

    // Switches Oversampling (1x, 2x, 4x, 8x)
    int btnY2 = btnY1 + 28;
    int w4 = (leftArea.getWidth() - 20) / 4;
    for (int i = 0; i < 4; ++i)
        buttons[3 + i]->setBounds(leftArea.getX() + 10 + (i * w4), btnY2, w4 - 4, 22);

    // Switches Mode (Stereo, M/S, Multiband)
    int btnY3 = btnY2 + 28;
    int w3 = (leftArea.getWidth() - 20) / 3;
    for (int i = 0; i < 3; ++i)
        buttons[7 + i]->setBounds(leftArea.getX() + 10 + (i * w3), btnY3, w3 - 4, 22);

    // --- PANEL KANAN (DRIVE) ---
    int rightY = rightArea.getY() + 145; // Setelah area VU Meter Drive

    // Knobs Drive, Tone, Output
    labels[4]->setBounds(rightArea.getX() + 15, rightY, 60, 14);
    knobs[4]->setBounds(rightArea.getX() + 20, rightY + 16, 50, 50);

    labels[5]->setBounds(rightArea.getX() + 105, rightY, 60, 14);
    knobs[5]->setBounds(rightArea.getX() + 110, rightY + 16, 50, 50);

    labels[6]->setBounds(rightArea.getX() + 195, rightY, 60, 14);
    knobs[6]->setBounds(rightArea.getX() + 200, rightY + 16, 50, 50);

    // Switches Drive Mode (Growl, Odd, Tape)
    int driveBtnY = rightY + 75;
    for (int i = 0; i < 3; ++i)
        buttons[10 + i]->setBounds(rightArea.getX() + 10 + (i * w3), driveBtnY, w3 - 4, 22);

    // Mix & Character
    int mixY = driveBtnY + 32;
    labels[7]->setBounds(rightArea.getX() + 35, mixY, 60, 14);
    knobs[7]->setBounds(rightArea.getX() + 40, mixY + 16, 50, 50);

    labels[8]->setBounds(rightArea.getX() + 165, mixY, 80, 14);
    knobs[8]->setBounds(rightArea.getX() + 180, mixY + 16, 50, 50);

    // HPF & LPF
    int filterY = mixY + 75;
    labels[9]->setBounds(rightArea.getX() + 35, filterY, 60, 14);
    knobs[9]->setBounds(rightArea.getX() + 40, filterY + 16, 50, 50);

    labels[10]->setBounds(rightArea.getX() + 165, filterY, 60, 14);
    knobs[10]->setBounds(rightArea.getX() + 180, filterY + 16, 50, 50);
}