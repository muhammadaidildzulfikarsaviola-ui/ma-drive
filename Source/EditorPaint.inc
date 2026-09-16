void MaDriveAudioProcessorEditor::paint(juce::Graphics& g)
{
    const int w = getWidth();
    const int h = getHeight();
    const int pw = juce::jlimit(215, 245, w / 4);
    const int lx = 28;
    const int rx = w - pw - 28;
    const int cx = lx + pw + 18;
    const int cw = juce::jmax(260, rx - cx - 18);

    g.fillAll(juce::Colour(0xff777168));

    auto frame = getLocalBounds().reduced(12);
    g.setColour(juce::Colour(0xff121313));
    g.fillRoundedRectangle(frame.toFloat(), 12.0f);
    g.setColour(juce::Colour(0xff807a70));
    g.fillRoundedRectangle(frame.reduced(10).toFloat(), 8.0f);

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(30.0f).withStyle("Bold"));
    g.drawText("Ma Drive", 0, 13, w, 38, juce::Justification::centred);
    g.setFont(juce::FontOptions(11.0f));
    g.drawText("WADIDAW / SATURATION / CLIPPER", 0, 48, w, 18, juce::Justification::centred);

    g.setFont(juce::FontOptions(12.0f).withStyle("Bold"));
    g.drawText("SOFT CLIP", lx, 74, pw, 18, juce::Justification::centred);
    g.drawText("DRIVE", rx, 74, pw, 18, juce::Justification::centred);

    // One dedicated Gain Reduction VU meter. The needles change with process mode.
    const int meterW = juce::jmax(210, cw - 8);
    const int meterH = 118;
    const int meterX = cx + (cw - meterW) / 2;
    const int meterY = 70;

    g.setColour(juce::Colour(0xff111212));
    g.fillRoundedRectangle((float)meterX, (float)meterY, (float)meterW, (float)meterH, 8.0f);
    g.setColour(juce::Colour(0xffd9bd62));
    g.fillRoundedRectangle((float)meterX + 7.0f, (float)meterY + 7.0f,
                           (float)meterW - 14.0f, (float)meterH - 14.0f, 6.0f);

    const float centreX = meterX + meterW * 0.5f;
    const float centreY = meterY + meterH * 0.88f;
    const float radius = juce::jmin(meterW * 0.38f, meterH * 0.58f);
    const float a0 = juce::MathConstants<float>::pi * 1.20f;
    const float a1 = juce::MathConstants<float>::pi * 1.80f;

    g.setColour(juce::Colour(0xff171815));
    juce::Path arc;
    arc.addCentredArc(centreX, centreY, radius, radius, 0.0f, a0, a1, true);
    g.strokePath(arc, juce::PathStrokeType(2.0f));

    const float values[] = { -24.0f, -18.0f, -12.0f, -6.0f, 0.0f };
    for (int i = 0; i < 5; ++i)
    {
        const float t = i / 4.0f;
        const float a = a0 + (a1 - a0) * t;
        const float ri = radius - 2.0f;
        const float ro = radius - 10.0f;
        g.setColour(juce::Colour(0xff242522));
        g.drawLine(centreX + std::cos(a) * ri, centreY + std::sin(a) * ri,
                   centreX + std::cos(a) * ro, centreY + std::sin(a) * ro, 1.4f);
        g.setFont(juce::FontOptions(7.0f).withStyle("Bold"));
        const int tx = (int)(centreX + std::cos(a) * (radius - 20.0f)) - 14;
        const int ty = (int)(centreY + std::sin(a) * (radius - 20.0f)) - 5;
        g.drawText(juce::String(values[i], 0), tx, ty, 28, 10, juce::Justification::centred);
    }

    const int mode = (int)p.parameters.getRawParameterValue("processMode")->load();
    const int needleCount = mode == 0 ? 1 : (mode == 1 ? 2 : 3);
    const float threshold = juce::jlimit(-24.0f, 0.0f, p.thresholdForDisplay.load());

    g.setColour(juce::Colours::black);
    g.setFont(juce::FontOptions(12.0f).withStyle("Bold"));
    g.drawText("GAIN REDUCTION", meterX, meterY + 6, meterW, 15, juce::Justification::centred);

    for (int i = 0; i < needleCount; ++i)
    {
        float grDb = 0.0f;
        if (mode == 0)
            grDb = p.reduction.load();
        else
        {
            const float sourceDb = p.monitorPeaks[i].load();
            grDb = -juce::jmax(0.0f, sourceDb - threshold);
        }

        grDb = juce::jlimit(-24.0f, 0.0f, grDb);
        const float t = juce::jlimit(0.0f, 1.0f, -grDb / 24.0f);
        const float a = a1 - (a1 - a0) * t;
        const float length = radius - 16.0f;
        const float qx = centreX + (needleCount == 1 ? 0.0f : (i - (needleCount - 1) * 0.5f) * 24.0f);

        g.setColour(juce::Colour(0xffff8a2a));
        g.drawLine(qx, centreY, qx + std::cos(a) * length, centreY + std::sin(a) * length, 2.2f);
        g.fillEllipse(qx - 2.5f, centreY - 2.5f, 5.0f, 5.0f);

        const juce::String label = mode == 0 ? "ST" :
                                   (mode == 1 ? (i == 0 ? "M" : "S") :
                                    (i == 0 ? "LOW" : (i == 1 ? "MID" : "HIGH")));
        g.setColour(juce::Colours::black);
        g.setFont(juce::FontOptions(7.0f).withStyle("Bold"));
        g.drawText(label, (int)qx - 22, meterY + meterH - 18, 44, 10, juce::Justification::centred);
    }

    // Central signal field.
    const int displayBottom = h - 108;
    auto display = juce::Rectangle<int>(cx, 205, cw, juce::jmax(185, displayBottom - 205));
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(display.toFloat(), 4.0f);

    g.setColour(juce::Colour(0xff414141));
    for (int i = 1; i < 7; ++i)
        g.drawHorizontalLine(display.getY() + i * display.getHeight() / 7,
                             (float)display.getX(), (float)display.getRight());
    for (int i = 1; i < 5; ++i)
        g.drawVerticalLine(display.getX() + i * display.getWidth() / 5,
                           (float)display.getY(), (float)display.getBottom());

    // Live signal numbers are deliberately inside the field.
    g.setColour(juce::Colour(0xffff8a2a));
    g.setFont(juce::FontOptions(10.0f).withStyle("Bold"));
    g.drawText("INPUT  " + juce::String(p.inputPeak.load(), 1) + " dB",
               display.getX() + 10, display.getY() + 8, display.getWidth() / 3 - 10, 16,
               juce::Justification::left);
    g.drawText("GR  " + juce::String(p.reduction.load(), 1) + " dB",
               display.getX() + display.getWidth() / 3, display.getY() + 8,
               display.getWidth() / 3, 16, juce::Justification::centred);
    g.drawText("OUTPUT  " + juce::String(p.outputPeak.load(), 1) + " dB",
               display.getX() + 2 * display.getWidth() / 3, display.getY() + 8,
               display.getWidth() / 3 - 10, 16, juce::Justification::right);

    const float thresholdT = juce::jlimit(0.0f, 1.0f, -threshold / 24.0f);
    const float thresholdY = display.getY() + display.getHeight() * thresholdT;
    const float dashPattern[] = { 5.0f, 4.0f };
    g.setColour(juce::Colour(0xffff8a2a));
    g.drawDashedLine(juce::Line<float>((float)display.getX(), thresholdY,
                                       (float)display.getRight(), thresholdY),
                     dashPattern, 2, 0.0f);
    g.setFont(juce::FontOptions(8.0f).withStyle("Bold"));
    g.drawText("THRESHOLD " + juce::String(threshold, 1) + " dB",
               display.getX() + 8, display.getY() + 28, 150, 12, juce::Justification::left);

    // Static transfer reference.
    juce::Path transfer;
    for (int i = 0; i <= 80; ++i)
    {
        const float t = i / 80.0f;
        const float xx = display.getX() + 24.0f + t * (display.getWidth() - 48.0f);
        const float shaped = 0.5f + 0.42f * std::tanh((t - 0.5f) * 3.0f);
        const float yy = display.getBottom() - 24.0f - shaped * (display.getHeight() - 58.0f);
        if (i == 0) transfer.startNewSubPath(xx, yy);
        else transfer.lineTo(xx, yy);
    }
    g.setColour(juce::Colour(0xff555555));
    g.strokePath(transfer, juce::PathStrokeType(1.0f));

    // Live audio history.
    juce::Path inputPath, outputPath;
    const int write = p.historyWrite.load();
    for (int i = 0; i < 256; ++i)
    {
        const int index = (write + i) & 255;
        const float inputAmp = juce::jlimit(0.0f, 1.0f,
            juce::Decibels::decibelsToGain(juce::jlimit(-48.0f, 6.0f, p.inputHistory[index])));
        const float outputAmp = juce::jlimit(0.0f, 1.0f,
            juce::Decibels::decibelsToGain(juce::jlimit(-48.0f, 6.0f, p.outputHistory[index])));
        const float xx = display.getX() + display.getWidth() * i / 255.0f;
        const float yyIn = display.getCentreY() - inputAmp * display.getHeight() * 0.27f;
        const float yyOut = display.getCentreY() + outputAmp * display.getHeight() * 0.27f;
        if (i == 0)
        {
            inputPath.startNewSubPath(xx, yyIn);
            outputPath.startNewSubPath(xx, yyOut);
        }
        else
        {
            inputPath.lineTo(xx, yyIn);
            outputPath.lineTo(xx, yyOut);
        }
    }
    g.setColour(juce::Colour(0x99eeeeee));
    g.strokePath(inputPath, juce::PathStrokeType(1.1f));
    g.setColour(juce::Colour(0x99f3372f));
    g.strokePath(outputPath, juce::PathStrokeType(1.5f));

    const int monitorY = display.getBottom() - 31;
    g.setColour(juce::Colour(0xff777777));
    g.setFont(juce::FontOptions(8.0f).withStyle("Bold"));
    g.drawText(mode == 0 ? "STEREO SIGNAL" : (mode == 1 ? "M / S SIGNAL" : "MULTI BAND SIGNAL"),
               display.getX() + 8, monitorY, display.getWidth() - 16, 12,
               juce::Justification::centred);

    const int clipStyle = (int)p.parameters.getRawParameterValue("clipStyle")->load();
    const int oversampling = (int)p.parameters.getRawParameterValue("oversampling")->load();
    const int driveMode = (int)p.parameters.getRawParameterValue("driveMode")->load();

    for (int i = 0; i < 3; ++i)
        buttons[i]->setColour(juce::TextButton::buttonColourId,
                              i == clipStyle ? juce::Colour(0xffff8a2a) : juce::Colour(0xff151616));
    for (int i = 0; i < 4; ++i)
        buttons[3 + i]->setColour(juce::TextButton::buttonColourId,
                                  i == oversampling ? juce::Colour(0xffff8a2a) : juce::Colour(0xff151616));
    for (int i = 0; i < 3; ++i)
        buttons[7 + i]->setColour(juce::TextButton::buttonColourId,
                                  i == mode ? juce::Colour(0xffff8a2a) : juce::Colour(0xff151616));
    for (int i = 0; i < 3; ++i)
        buttons[10 + i]->setColour(juce::TextButton::buttonColourId,
                                   i == driveMode ? juce::Colour(0xffff8a2a) : juce::Colour(0xff151616));

    buttons[14]->setColour(juce::TextButton::buttonColourId,
                           activeB ? juce::Colour(0xff151616) : juce::Colour(0xffff8a2a));
    buttons[15]->setColour(juce::TextButton::buttonColourId,
                           activeB ? juce::Colour(0xffff8a2a) : juce::Colour(0xff151616));

    if (p.parameters.getRawParameterValue("bypass")->load() > 0.5f)
    {
        buttons[13]->setButtonText("BYPASSED");
        buttons[13]->setColour(juce::TextButton::buttonColourId, juce::Colour(0xffff8a2a));
    }
    else
    {
        buttons[13]->setButtonText("BYPASS");
        buttons[13]->setColour(juce::TextButton::buttonColourId, juce::Colour(0xff151616));
    }
}
