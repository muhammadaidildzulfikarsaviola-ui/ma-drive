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

    auto analog = [&](int x, int y, int mw, int mh, float inDb, float outDb, float grDb, bool showGR)
    {
        g.setColour(juce::Colour(0xff111212));
        g.fillRoundedRectangle((float)x, (float)y, (float)mw, (float)mh, 8.0f);
        g.setColour(juce::Colour(0xffd9bd62));
        g.fillRoundedRectangle((float)x + 7.0f, (float)y + 7.0f,
                               (float)mw - 14.0f, (float)mh - 14.0f, 6.0f);

        const float centreX = x + mw * 0.5f;
        const float centreY = y + mh * 0.88f;
        const float radius = juce::jmin(mw * 0.38f, mh * 0.58f);
        const float a0 = juce::MathConstants<float>::pi * 1.20f;
        const float a1 = juce::MathConstants<float>::pi * 1.80f;

        g.setColour(juce::Colour(0xff171815));
        juce::Path arc;
        arc.addCentredArc(centreX, centreY, radius, radius, 0.0f, a0, a1, true);
        g.strokePath(arc, juce::PathStrokeType(2.0f));

        const float values[] = { -20.0f, -12.0f, -6.0f, 0.0f, 3.0f };
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
            const int tx = (int)(centreX + std::cos(a) * (radius - 20.0f)) - 13;
            const int ty = (int)(centreY + std::sin(a) * (radius - 20.0f)) - 5;
            g.drawText(juce::String(values[i], 0), tx, ty, 26, 10, juce::Justification::centred);
        }

        auto needle = [&](float db, juce::Colour colour, float length)
        {
            const float t = juce::jlimit(0.0f, 1.0f, (db + 20.0f) / 23.0f);
            const float a = a0 + (a1 - a0) * t;
            const float ex = centreX + std::cos(a) * length;
            const float ey = centreY + std::sin(a) * length;
            g.setColour(colour);
            g.drawLine(centreX, centreY, ex, ey, 2.2f);
            g.fillEllipse(centreX - 2.5f, centreY - 2.5f, 5.0f, 5.0f);
        };

        needle(inDb, juce::Colour(0xfff4f0df), radius - 13.0f);
        needle(outDb, juce::Colour(0xffd9362e), radius - 16.0f);

        if (showGR)
        {
            const float t = juce::jlimit(0.0f, 1.0f, grDb / 20.0f);
            const float a = a1 - (a1 - a0) * t;
            g.setColour(juce::Colour(0xffff8a2a));
            g.drawLine(centreX, centreY,
                       centreX + std::cos(a) * (radius - 21.0f),
                       centreY + std::sin(a) * (radius - 21.0f), 1.7f);
        }

        g.setColour(juce::Colours::black);
        g.setFont(juce::FontOptions(13.0f).withStyle("Bold"));
        g.drawText("VU", x, y + 44, mw, 18, juce::Justification::centred);
        g.setFont(juce::FontOptions(6.5f).withStyle("Bold"));
        g.setColour(juce::Colours::black);
        g.drawText("INPUT", x + 5, y + mh - 17, mw / 3, 10, juce::Justification::centred);
        g.setColour(juce::Colour(0xffd9362e));
        g.drawText("OUTPUT", x + mw / 3, y + mh - 17, mw / 3, 10, juce::Justification::centred);
        if (showGR)
        {
            g.setColour(juce::Colour(0xffff8a2a));
            g.drawText("GR", x + 2 * mw / 3, y + mh - 17, mw / 3, 10, juce::Justification::centred);
        }
    };

    // A/B now has its own strip, so it never covers the meters.
    analog(lx + 8, 108, pw - 16, 118,
           p.rackPeaks[0].load(), p.rackPeaks[1].load(), p.softClipGR.load(), true);
    analog(rx + 8, 108, pw - 16, 118,
           p.rackPeaks[2].load(), p.rackPeaks[3].load(), 0.0f, false);

    // Central monitor.
    const int displayBottom = h - 108;
    auto display = juce::Rectangle<int>(cx, 100, cw, juce::jmax(285, displayBottom - 100));
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(display.toFloat(), 4.0f);

    // Grid and a visible saturation transfer guide so the display is not visually empty.
    g.setColour(juce::Colour(0xff414141));
    for (int i = 1; i < 7; ++i)
        g.drawHorizontalLine(display.getY() + i * display.getHeight() / 7,
                             (float)display.getX(), (float)display.getRight());
    for (int i = 1; i < 5; ++i)
        g.drawVerticalLine(display.getX() + i * display.getWidth() / 5,
                           (float)display.getY(), (float)display.getBottom());

    const float threshold = juce::jlimit(-24.0f, 0.0f, p.thresholdForDisplay.load());
    const float thresholdT = juce::jlimit(0.0f, 1.0f, -threshold / 24.0f);
    const float thresholdY = display.getY() + display.getHeight() * thresholdT;
    const float dashPattern[] = { 5.0f, 4.0f };
    g.setColour(juce::Colour(0xffff8a2a));
    g.drawDashedLine(juce::Line<float>((float)display.getX(), thresholdY,
                                       (float)display.getRight(), thresholdY),
                     dashPattern, 2, 0.0f);
    g.setFont(juce::FontOptions(9.0f).withStyle("Bold"));
    g.drawText("THRESHOLD " + juce::String(threshold, 1) + " dB",
               display.getX() + 8, display.getY() + 7, 150, 14, juce::Justification::left);

    // Static transfer reference curve.
    juce::Path transfer;
    for (int i = 0; i <= 80; ++i)
    {
        const float t = i / 80.0f;
        const float x = display.getX() + 24.0f + t * (display.getWidth() - 48.0f);
        const float shaped = 0.5f + 0.42f * std::tanh((t - 0.5f) * 3.0f);
        const float y = display.getBottom() - 24.0f - shaped * (display.getHeight() - 48.0f);
        if (i == 0) transfer.startNewSubPath(x, y);
        else transfer.lineTo(x, y);
    }
    g.setColour(juce::Colour(0xff555555));
    g.strokePath(transfer, juce::PathStrokeType(1.0f));
    g.setFont(juce::FontOptions(8.0f));
    g.setColour(juce::Colour(0xff777777));
    g.drawText("SATURATION TRANSFER", display.getX() + 8, display.getBottom() - 18,
               display.getWidth() - 16, 12, juce::Justification::centred);

    // Live history waveform, when there is actual signal.
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
        const float yyIn = display.getCentreY() - inputAmp * display.getHeight() * 0.28f;
        const float yyOut = display.getCentreY() + outputAmp * display.getHeight() * 0.28f;
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

    // Clear monitor panel. One / two / three needles according to process mode.
    const int mode = (int)p.parameters.getRawParameterValue("processMode")->load();
    const int monitorH = 70;
    const int monitorY = display.getBottom() - monitorH - 10;
    const int monitorW = display.getWidth() - 20;
    const int monitorX = display.getX() + 10;
    const int needleCount = mode == 0 ? 1 : (mode == 1 ? 2 : 3);

    g.setColour(juce::Colour(0xff090a0a));
    g.fillRoundedRectangle((float)monitorX, (float)monitorY, (float)monitorW, (float)monitorH, 6.0f);
    g.setColour(juce::Colour(0xff9a9a9a));
    g.setFont(juce::FontOptions(9.0f).withStyle("Bold"));
    g.drawText(mode == 0 ? "STEREO MONITOR" : (mode == 1 ? "M / S MONITOR" : "MULTI BAND MONITOR"),
               monitorX + 8, monitorY + 4, monitorW - 16, 13, juce::Justification::centred);

    for (int i = 0; i < needleCount; ++i)
    {
        const float level = juce::jlimit(-24.0f, 3.0f, p.monitorPeaks[i].load());
        const float t = (level + 24.0f) / 27.0f;
        const float qx = monitorX + monitorW * (i + 0.5f) / needleCount;
        const float qy = monitorY + 46.0f;
        const float r = juce::jmin(monitorW / (float)needleCount * 0.27f, 20.0f);
        const float a = juce::MathConstants<float>::pi * (1.25f + t * 0.50f);
        g.setColour(juce::Colour(0xff555555));
        g.drawLine(qx - r, qy, qx + r, qy, 1.0f);
        g.setColour(juce::Colour(0xffff8a2a));
        g.drawLine(qx, qy, qx + std::cos(a) * r, qy + std::sin(a) * r, 1.8f);
        g.fillEllipse(qx - 2.0f, qy - 2.0f, 4.0f, 4.0f);

        const juce::String label = mode == 0 ? "ST" :
                                   (mode == 1 ? (i == 0 ? "M" : "S") :
                                    (i == 0 ? "LOW" : (i == 1 ? "MID" : "HIGH")));
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(7.0f).withStyle("Bold"));
        g.drawText(label, (int)qx - 22, monitorY + 54, 44, 11, juce::Justification::centred);
    }

    // Live numeric readout is separated from the needles.
    g.setColour(juce::Colour(0xffff8a2a));
    g.setFont(juce::FontOptions(7.0f));
    g.drawText("IN " + juce::String(p.inputPeak.load(), 1) +
               " dB   GR " + juce::String(p.reduction.load(), 1) +
               " dB   OUT " + juce::String(p.outputPeak.load(), 1) + " dB",
               display.getX() + 4, monitorY - 15, display.getWidth() - 8, 12,
               juce::Justification::centred);

    const int clipStyle = (int)p.parameters.getRawParameterValue("clipStyle")->load();
    const int oversampling = (int)p.parameters.getRawParameterValue("oversampling")->load();
    const int driveMode = (int)p.parameters.getRawParameterValue("driveMode")->load();

    for (int i = 0; i < 3; ++i) buttons[i]->setToggleState(i == clipStyle, juce::dontSendNotification);
    for (int i = 0; i < 4; ++i) buttons[3 + i]->setToggleState(i == oversampling, juce::dontSendNotification);
    for (int i = 0; i < 3; ++i) buttons[7 + i]->setToggleState(i == mode, juce::dontSendNotification);
    for (int i = 0; i < 3; ++i) buttons[10 + i]->setToggleState(i == driveMode, juce::dontSendNotification);
    buttons[14]->setToggleState(!activeB, juce::dontSendNotification);
    buttons[15]->setToggleState(activeB, juce::dontSendNotification);
    buttons[13]->setButtonText(p.parameters.getRawParameterValue("bypass")->load() > 0.5f ? "BYPASS ON" : "BYPASS");
}
