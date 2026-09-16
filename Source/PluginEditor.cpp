#include "PluginProcessor.h"
#include "PluginEditor.h"

// ==============================================================================
// CONSTRUCTOR & DESTRUCTOR
// ==============================================================================
MaDriveAudioProcessorEditor::MaDriveAudioProcessorEditor(MaDriveAudioProcessor& p_)
    : AudioProcessorEditor(&p_), p(p_)
{
    // Helper Lambda Konversi Jam ke Radian (Jam 12 = 0, Jam 7 = -135deg, Jam 5 = +135deg)
    auto setKnobAngles = [](juce::Slider* slider, float startClockHour, float endClockHour) {
        float startAngle = juce::MathConstants<float>::pi * ((startClockHour - 12.0f) / 6.0f);
        float endAngle   = juce::MathConstants<float>::pi * ((endClockHour - 12.0f) / 6.0f);
        slider->setRotaryParameters(startAngle, endAngle, true);
    };

    // Inisialisasi 12 Knobs
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

    // Set Rentang Putaran Sesuai Spesifikasi Jam
    for (int i = 0; i <= 3; ++i) setKnobAngles(knobs[i], 7.0f, 5.0f); // Soft Clip (Default Jam 12)
    setKnobAngles(knobs[4], 7.0f, 5.0f);  // Drive
    setKnobAngles(knobs[5], 7.0f, 5.0f);  // Tone
    setKnobAngles(knobs[6], 7.0f, 5.0f);  // Output
    setKnobAngles(knobs[7], 7.0f, 5.0f);  // Mix
    setKnobAngles(knobs[8], 7.0f, 5.0f);  // Character
    setKnobAngles(knobs[9], 7.0f, 5.0f);  // HPF
    setKnobAngles(knobs[10], 7.0f, 5.0f); // LPF
    setKnobAngles(knobs[11], 7.0f, 5.0f); // Trim

    const char* knobNames[] = {
        "INPUT", "THRESHOLD", "KNEE", "OUTPUT",
        "DRIVE", "TONE", "OUTPUT",
        "MIX", "CHARACTER", "HPF", "LPF", "TRIM"
    };
    for (int i = 0; i < 12; ++i) labels[i]->setText(knobNames[i], juce::dontSendNotification);

    // Inisialisasi 17 Saklar / Buttons
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

    // Setting Saklar Interaktif (Radio Button Groups)
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

// ==============================================================================
// IMPLEMENTASI PAINT (CANVAS & VU METER)
// ==============================================================================
void MaDriveAudioProcessorEditor::paint(juce::Graphics& g)
{
    // 1. Base Metal Chassis
    g.fillAll(juce::Colour(0xff939699));

    // Baut Hardware Sudut
    auto drawScrew = [&g](float x, float y) {
        g.setColour(juce::Colour(0xff333537));
        g.fillEllipse(x, y, 9.0f, 9.0f);
        g.setColour(juce::Colour(0xff151617));
        g.drawEllipse(x, y, 9.0f, 9.0f, 1.0f);
        g.drawLine(x + 2.0f, y + 4.5f, x + 7.0f, y + 4.5f, 1.0f);
    };
    drawScrew(6, 6);
    drawScrew((float)getWidth() - 15, 6);
    drawScrew(6, (float)getHeight() - 15);
    drawScrew((float)getWidth() - 15, (float)getHeight() - 15);

    // 2. Header Title
    g.setColour(juce::Colours::black);
    g.setFont(juce::Font(26.0f, juce::Font::bold));
    g.drawFittedText("WADIDAW", juce::Rectangle<int>(0, 4, getWidth(), 28), juce::Justification::centred, 1);
    g.setColour(juce::Colour(0xffd62828));
    g.fillRect(juce::Rectangle<float>((float)getWidth() * 0.5f - 55.0f, 30.0f, 110.0f, 2.0f));

    // 3. Panel Frames
    auto boundsArea = getLocalBounds().reduced(10, 2);
    boundsArea.removeFromTop(45);
    boundsArea.removeFromBottom(45);

    int panelWidth = boundsArea.getWidth() / 3;
    auto leftArea   = boundsArea.removeFromLeft(panelWidth).reduced(2);
    auto rightArea  = boundsArea.removeFromRight(panelWidth).reduced(2);
    auto centerArea = boundsArea.reduced(2);

    g.setColour(juce::Colour(0xff1d1f20));
    g.drawRect(leftArea, 2.0f);
    g.drawRect(rightArea, 2.0f);
    g.drawRect(centerArea, 2.0f);

    auto drawHeaderWithBypass = [&g](juce::Rectangle<int> area, const char* title) {
        g.setColour(juce::Colour(0xffff8a2a));
        g.fillRect(area.getX() + 8, area.getY() + 8, 10, 10);
        g.setColour(juce::Colour(0xff111111));
        g.drawRect(area.getX() + 8, area.getY() + 8, 10, 10, 1);

        g.setFont(juce::Font(13.0f, juce::Font::bold));
        g.drawFittedText(title, juce::Rectangle<int>(area.getX() + 24, area.getY() + 4, 120, 18), juce::Justification::left, 1);
    };

    drawHeaderWithBypass(leftArea, "SOFT CLIP");
    drawHeaderWithBypass(rightArea, "DRIVE");

    // 4. Render VU Meter Analog Presisi
    auto drawLargeVUMeter = [&g](juce::Rectangle<int> rect, float peakDb, const char* label) {
        g.setColour(juce::Colour(0xff121314));
        g.fillRoundedRectangle(rect.toFloat(), 6.0f);
        g.setColour(juce::Colour(0xff3a3d40));
        g.drawRoundedRectangle(rect.toFloat(), 6.0f, 1.5f);

        auto inner = rect.reduced(4);
        juce::ColourGradient bgGrad(juce::Colour(0xfffceabb), (float)inner.getX(), (float)inner.getY(),
                                    juce::Colour(0xfff8b500), (float)inner.getX(), (float)inner.getBottom(), false);
        g.setGradientFill(bgGrad);
        g.fillRoundedRectangle(inner.toFloat(), 4.0f);

        auto pivot = juce::Point<float>((float)inner.getCentreX(), (float)inner.getBottom() + 18.0f);
        float radius = (float)inner.getHeight() * 0.98f;

        auto dbToAngle = [](float db) -> float {
            float norm = juce::jlimit(0.0f, 1.0f, juce::jmap(db, -20.0f, 3.0f, 0.0f, 1.0f));
            return juce::jmap(norm, -0.785f, 0.785f);
        };

        juce::Path mainArc, redArc;
        float startAng = dbToAngle(-20.0f) - juce::MathConstants<float>::halfPi;
        float zeroAng  = dbToAngle(0.0f)   - juce::MathConstants<float>::halfPi;
        float endAng   = dbToAngle(3.0f)   - juce::MathConstants<float>::halfPi;

        mainArc.addCentredArc(pivot.x, pivot.y, radius * 0.75f, radius * 0.75f, 0.0f, startAng, zeroAng, true);
        redArc.addCentredArc(pivot.x, pivot.y, radius * 0.75f, radius * 0.75f, 0.0f, zeroAng, endAng, true);

        g.setColour(juce::Colour(0xff222222));
        g.strokePath(mainArc, juce::PathStrokeType(1.5f));

        g.setColour(juce::Colour(0xffcc1111));
        g.strokePath(redArc, juce::PathStrokeType(3.0f));

        struct VUTick { float db; const char* text; bool isMajor; };
        const VUTick ticks[] = {
            {-20.0f, "-20", true}, {-10.0f, "-10", true}, {-7.0f, "-7", true},
            {-5.0f, "-5", true},   {-3.0f, "-3", true},   {-2.0f, "", false},
            {-1.0f, "-1", true},   { 0.0f, "0", true},    { 1.0f, "+1", true},
            { 2.0f, "+2", true},   { 3.0f, "+3", true}
        };

        g.setFont(juce::Font(9.0f, juce::Font::bold));

        for (const auto& t : ticks)
        {
            float angle = dbToAngle(t.db);
            bool isRed = (t.db >= 0.0f);

            float innerR = radius * 0.73f;
            float outerR = t.isMajor ? radius * 0.81f : radius * 0.77f;

            float x1 = pivot.x + std::sin(angle) * innerR;
            float y1 = pivot.y - std::cos(angle) * innerR;
            float x2 = pivot.x + std::sin(angle) * outerR;
            float y2 = pivot.y - std::cos(angle) * outerR;

            g.setColour(isRed ? juce::Colour(0xffcc1111) : juce::Colour(0xff222222));
            g.drawLine(x1, y1, x2, y2, t.isMajor ? 1.5f : 1.0f);

            if (t.text[0] != '\0')
            {
                float textR = radius * 0.58f;
                float tx = pivot.x + std::sin(angle) * textR;
                float ty = pivot.y - std::cos(angle) * textR;

                juce::Rectangle<float> textBounds(tx - 12.0f, ty - 6.0f, 24.0f, 12.0f);
                g.drawText(t.text, textBounds, juce::Justification::centred, false);
            }
        }

        g.setColour(juce::Colour(0xff111111));
        g.setFont(juce::Font(15.0f, juce::Font::bold));
        g.drawText("VU", juce::Rectangle<float>(inner.getX(), inner.getY() + 8, inner.getWidth(), 18), juce::Justification::centred, false);

        g.setFont(juce::Font(8.5f, juce::Font::bold));
        g.drawText(label, juce::Rectangle<float>(inner.getX(), inner.getBottom() - 16, inner.getWidth(), 12), juce::Justification::centred, false);

        float needleAngle = dbToAngle(peakDb);
        float nx = pivot.x + std::sin(needleAngle) * (radius * 0.85f);
        float ny = pivot.y - std::cos(needleAngle) * (radius * 0.85f);

        // Shadow & Needle
        g.setColour(juce::Colour(0x33000000));
        g.drawLine(pivot.x + 2.0f, pivot.y + 2.0f, nx + 2.0f, ny + 2.0f, 1.5f);

        g.setColour(juce::Colour(0xff151515));
        g.drawLine(pivot.x, pivot.y, nx, ny, 1.8f);

        auto pivotRect = juce::Rectangle<float>(pivot.x - 10.0f, pivot.y - 10.0f, 20.0f, 20.0f);
        g.setColour(juce::Colour(0xff1c1d1e));
        g.fillEllipse(pivotRect);
        g.setColour(juce::Colour(0xff4f5255));
        g.drawEllipse(pivotRect, 1.0f);
    };

    drawLargeVUMeter(juce::Rectangle<int>(leftArea.getX() + 6, leftArea.getY() + 24, leftArea.getWidth() - 12, 115), p.inputPeak.load(), "INPUT / THRESHOLD");
    drawLargeVUMeter(juce::Rectangle<int>(rightArea.getX() + 6, rightArea.getY() + 24, rightArea.getWidth() - 12, 115), p.outputPeak.load(), "INPUT / OUTPUT");

    // 5. Display Screen & Meter Center
    auto displayScreen = centerArea.removeFromTop(210).reduced(4);
    g.setColour(juce::Colour(0xff0b0c0d));
    g.fillRect(displayScreen);
    g.setColour(juce::Colour(0xff2a2d30));
    g.drawRect(displayScreen, 1.5f);

    juce::Path wavePath;
    wavePath.startNewSubPath((float)displayScreen.getX() + 10, (float)displayScreen.getY() + 70);
    for (int x = 10; x < displayScreen.getWidth() - 10; x += 4)
    {
        float sampleY = std::sin((float)x * 0.08f) * 40.0f;
        wavePath.lineTo((float)(displayScreen.getX() + x), (float)(displayScreen.getY() + 70) + sampleY);
    }
    g.setColour(juce::Colours::white);
    g.strokePath(wavePath, juce::PathStrokeType(1.5f));

    auto meterArea = centerArea.reduced(4);
    g.setColour(juce::Colour(0xff121314));
    g.fillRect(meterArea);

    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawFittedText("THRESHOLD MONITOR", juce::Rectangle<int>(meterArea.getX(), meterArea.getY() + 4, meterArea.getWidth(), 14), juce::Justification::centred, 1);

    auto grBarArea = juce::Rectangle<int>(meterArea.getCentreX() - 70, meterArea.getY() + 26, 140, 16);
    g.setColour(juce::Colour(0xff222425));
    g.fillRect(grBarArea);

    float grDb = p.reduction.load();
    int numLeds = 12;
    int activeLeds = juce::jlimit(0, numLeds, (int)juce::jmap(std::abs(grDb), 0.0f, 24.0f, 0.0f, (float)numLeds));

    for (int i = 0; i < numLeds; ++i)
    {
        auto ledRect = juce::Rectangle<int>(grBarArea.getX() + (i * 11) + 2, grBarArea.getY() + 2, 8, 12);
        if (i < activeLeds) g.setColour(juce::Colour(0xffff6600));
        else g.setColour(juce::Colour(0xff2d1400));

        g.fillRect(ledRect);
    }

    g.setColour(juce::Colours::black);
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.drawFittedText("WADIDAW", juce::Rectangle<int>(12, getHeight() - 36, 120, 25), juce::Justification::left, 1);
}

// ==============================================================================
// IMPLEMENTASI RESIZED (POSISI UTAMA KOMPONEN)
// ==============================================================================
void MaDriveAudioProcessorEditor::resized()
{
    auto boundsArea = getLocalBounds().reduced(10, 2);

    buttons[14]->setBounds(16, 12, 35, 22); // Switch A
    buttons[15]->setBounds(55, 12, 35, 22); // Switch B

    boundsArea.removeFromTop(45);
    auto footerArea = boundsArea.removeFromBottom(45);

    buttons[16]->setBounds(footerArea.getCentreX() - 60, footerArea.getY() + 8, 120, 26); // SUPPORT ME
    buttons[13]->setBounds(footerArea.getRight() - 170, footerArea.getY() + 8, 90, 26);    // BYPASS

    knobs[11]->setBounds(footerArea.getRight() - 60, footerArea.getY() + 2, 32, 32);       // Trim Knob
    labels[11]->setBounds(footerArea.getRight() - 70, footerArea.getY() + 32, 52, 12);

    int panelWidth = boundsArea.getWidth() / 3;
    auto leftArea   = boundsArea.removeFromLeft(panelWidth).reduced(2);
    auto rightArea  = boundsArea.removeFromRight(panelWidth).reduced(2);

    // --- LEFT PANEL (SOFT CLIP) ---
    int leftY = leftArea.getY() + 145;

    labels[0]->setBounds(leftArea.getX() + 25, leftY, 70, 14);
    knobs[0]->setBounds(leftArea.getX() + 35, leftY + 16, 50, 50);

    labels[1]->setBounds(leftArea.getX() + 145, leftY, 80, 14);
    knobs[1]->setBounds(leftArea.getX() + 160, leftY + 16, 50, 50);

    labels[2]->setBounds(leftArea.getX() + 25, leftY + 80, 70, 14);
    knobs[2]->setBounds(leftArea.getX() + 35, leftY + 96, 50, 50);

    labels[3]->setBounds(leftArea.getX() + 145, leftY + 80, 80, 14);
    knobs[3]->setBounds(leftArea.getX() + 160, leftY + 96, 50, 50);

    int btnY1 = leftY + 165;
    buttons[0]->setBounds(leftArea.getX() + 10, btnY1, 80, 22);
    buttons[1]->setBounds(leftArea.getX() + 96, btnY1, 80, 22);
    buttons[2]->setBounds(leftArea.getX() + 182, btnY1, 80, 22);

    int btnY2 = btnY1 + 28;
    int w4 = (leftArea.getWidth() - 20) / 4;
    for (int i = 0; i < 4; ++i)
        buttons[3 + i]->setBounds(leftArea.getX() + 10 + (i * w4), btnY2, w4 - 4, 22);

    int btnY3 = btnY2 + 28;
    int w3 = (leftArea.getWidth() - 20) / 3;
    for (int i = 0; i < 3; ++i)
        buttons[7 + i]->setBounds(leftArea.getX() + 10 + (i * w3), btnY3, w3 - 4, 22);

    // --- RIGHT PANEL (DRIVE) ---
    int rightY = rightArea.getY() + 145;

    labels[4]->setBounds(rightArea.getX() + 15, rightY, 60, 14);
    knobs[4]->setBounds(rightArea.getX() + 20, rightY + 16, 50, 50);

    labels[5]->setBounds(rightArea.getX() + 105, rightY, 60, 14);
    knobs[5]->setBounds(rightArea.getX() + 110, rightY + 16, 50, 50);

    labels[6]->setBounds(rightArea.getX() + 195, rightY, 60, 14);
    knobs[6]->setBounds(rightArea.getX() + 200, rightY + 16, 50, 50);

    int driveBtnY = rightY + 75;
    for (int i = 0; i < 3; ++i)
        buttons[10 + i]->setBounds(rightArea.getX() + 10 + (i * w3), driveBtnY, w3 - 4, 22);

    int mixY = driveBtnY + 32;
    labels[7]->setBounds(rightArea.getX() + 35, mixY, 60, 14);
    knobs[7]->setBounds(rightArea.getX() + 40, mixY + 16, 50, 50);

    labels[8]->setBounds(rightArea.getX() + 165, mixY, 80, 14);
    knobs[8]->setBounds(rightArea.getX() + 180, mixY + 16, 50, 50);

    int filterY = mixY + 75;
    labels[9]->setBounds(rightArea.getX() + 35, filterY, 60, 14);
    knobs[9]->setBounds(rightArea.getX() + 40, filterY + 16, 50, 50);

    labels[10]->setBounds(rightArea.getX() + 165, filterY, 60, 14);
    knobs[10]->setBounds(rightArea.getX() + 180, filterY + 16, 50, 50);
}