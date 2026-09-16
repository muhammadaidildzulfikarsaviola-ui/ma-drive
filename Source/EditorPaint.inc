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
        const float centreY = y + mh * 0.80f;
        const float radius = juce::jmin(mw * 0.43f, mh * 0.68f);
        const float a0 = juce::MathConstants<float>::pi * 1.16f;
        const float a1 = juce::MathConstants<float>::pi * 1.84f;

        g.setColour(juce::Colour(0xff191a18));
        juce::Path arc;
        arc.addCentredArc(centreX, centreY, radius, radius, 0.0f, a0, a1, true);
        g.strokePath(arc, juce::PathStrokeType(2.0f));

        for (int i = 0; i <= 13; ++i)
        {
            const float a = a0 + (a1 - a0) * i / 13.0f;
            const float ri = radius - 2.0f;
            const float ro = radius - 11.0f;
            g.drawLine(centreX + std::cos(a) * ri, centreY + std::sin(a) * ri,
                       centreX + std::cos(a) * ro, centreY + std::sin(a) * ro, 1.1f);
            const float value = -20.0f + 23.0f * i / 13.0f;
            g.setFont(juce::FontOptions(7.0f).withStyle("Bold"));
            g.drawText(juce::String(value, 0), (int)(centreX + std::cos(a) * (radius - 25.0f)) - 12,
                       (int)(centreY + std::sin(a) * (radius - 25.0f)) - 5, 24, 10,
                       juce::Justification::centred);
        }

        auto needle = [&](float db, juce::Colour colour)
        {
            const float t = juce::jlimit(0.0f, 1.0f, (db + 20.0f) / 23.0f);
            const float a = a0 + (a1 - a0) * t;
            const float ex = centreX + std::cos(a) * (radius - 15.0f);
            const float ey = centreY + std::sin(a) * (radius - 15.0f);
            g.setColour(colour);
            g.drawLine(centreX, centreY, ex, ey, 2.2f);
            g.fillEllipse(centreX - 3.0f, centreY - 3.0f, 6.0f, 6.0f);
        };

        needle(inDb, juce::Colours::white);
        needle(outDb, juce::Colour(0xffef332d));

        if (showGR)
        {
            const float t = juce::jlimit(0.0f, 1.0f, grDb / 20.0f);
            const float a = a1 - (a1 - a0) * t;
            g.setColour(juce::Colour(0xffff8a2a));
            g.drawLine(centreX, centreY,
                       centreX + std::cos(a) * (radius - 19.0f),
                       centreY + std::sin(a) * (radius - 19.0f), 1.8f);
        }

        g.setColour(juce::Colours::black);
        g.setFont(juce::FontOptions(14.0f).withStyle("Bold"));
        g.drawText("VU", x, y + mh / 2 - 3, mw, 20, juce::Justification::centred);
        g.setFont(juce::FontOptions(7.0f).withStyle("Bold"));
        g.setColour(juce::Colours::black);
        g.drawText("INPUT", x + 6, y + mh - 19, mw / 3, 11, juce::Justification::centred);
        g.setColour(juce::Colour(0xffef332d));
        g.drawText("OUTPUT", x + mw / 3, y + mh - 19, mw / 3, 11, juce::Justification::centred);
        if (showGR)
        {
            g.setColour(juce::Colour(0xffff8a2a));
            g.drawText("GR", x + 2 * mw / 3, y + mh - 19, mw / 3, 11, juce::Justification::centred);
        }
    };

    analog(lx + 8, 97, pw - 16, 130,
           p.rackPeaks[0].load(), p.rackPeaks[1].load(), p.softClipGR.load(), true);
    analog(rx + 8, 97, pw - 16, 130,
           p.rackPeaks[2].load(), p.rackPeaks[3].load(), 0.0f, false);

    // Central monitor.
    auto display = juce::Rectangle<int>(cx, 94, cw, juce::jmax(300, h - 142));
    g.setColour(juce::Colours::black);
    g.fillRect(display);

    g.setColour(juce::Colour(0xff4a4a4a));
    for (int i = 1; i < 8; ++i)
        g.drawHorizontalLine(display.getY() + i * display.getHeight() / 8,
                             (float)display.getX(), (float)display.getRight());

    const float threshold = juce::jlimit(-24.0f, 0.0f, p.thresholdForDisplay.load());
    const float thresholdT = juce::jlimit(0.0f, 1.0f, -threshold / 24.0f);
    const float thresholdY = display.getY() + display.getHeight() * thresholdT;
    g.setColour(juce::Colour(0xffff8a2a));
    g.drawDashedLine({ (float)display.getX(), thresholdY, (float)display.getRight(), thresholdY }, { 5.0f, 4.0f }, 2, 0.0f);
    g.setFont(juce::FontOptions(10.0f));
    g.drawText("THRESHOLD " + juce::String(threshold, 1) + " dB",
               display.getX() + 7, (int)thresholdY - 15, 145, 14, juce::Justification::left);

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
        const float yyIn = display.getBottom() - inputAmp * display.getHeight() * 0.45f;
        const float yyOut = display.getBottom() - outputAmp * display.getHeight() * 0.45f;
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

    g.setColour(juce::Colour(0xffeeeeee));
    g.strokePath(inputPath, juce::PathStrokeType(1.2f));
    g.setColour(juce::Colour(0xfff3372f));
    g.strokePath(outputPath, juce::PathStrokeType(1.8f));

    const int mode = (int)p.parameters.getRawParameterValue("processMode")->load();
    const int monitorH = 66;
    const int monitorY = display.getBottom() - monitorH - 9;
    const int monitorW = display.getWidth() - 20;
    const int monitorX = display.getX() + 10;
    const int needleCount = mode == 0 ? 1 : (mode == 1 ? 2 : 3);

    g.setColour(juce::Colour(0xcc090a0a));
    g.fillRoundedRectangle((float)monitorX, (float)monitorY, (float)monitorW, (float)monitorH, 5.0f);
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(9.0f).withStyle("Bold"));
    g.drawText(mode == 0 ? "VU / STEREO" : (mode == 1 ? "VU / M/S" : "VU / MULTI BAND"),
               monitorX, monitorY + 3, monitorW, 13, juce::Justification::centred);

    for (int i = 0; i < needleCount; ++i)
    {
        const float level = juce::jlimit(-24.0f, 3.0f, p.monitorPeaks[i].load());
        const float t = (level + 24.0f) / 27.0f;
        const float qx = monitorX + monitorW * (i + 0.5f) / needleCount;
        const float qy = monitorY + monitorH - 11.0f;
        const float r = juce::jmin(monitorW / (float)needleCount * 0.32f, 23.0f);
        const float a = juce::MathConstants<float>::pi * (1.25f + t * 0.50f);
        g.setColour(juce::Colour(0xff666666));
        g.drawLine(qx - r, qy, qx + r, qy, 1.0f);
        g.setColour(juce::Colour(0xffff8a2a));
        g.drawLine(qx, qy, qx + std::cos(a) * r, qy + std::sin(a) * r, 1.8f);
        g.fillEllipse(qx - 2.0f, qy - 2.0f, 4.0f, 4.0f);

        const juce::String label = mode == 0 ? "ST" :
                                   (mode == 1 ? (i == 0 ? "M" : "S") :
                                    (i == 0 ? "LOW" : (i == 1 ? "MID" : "HIGH")));
        g.setColour(juce::Colours::white);
        g.drawText(label, (int)qx - 22, monitorY + monitorH - 17, 44, 12, juce::Justification::centred);
    }

    g.setColour(juce::Colour(0xffff8a2a));
    g.setFont(juce::FontOptions(8.0f));
    g.drawText("IN " + juce::String(p.inputPeak.load(), 1) + " dB   GR " +
               juce::String(p.reduction.load(), 1) + " dB   OUT " +
               juce::String(p.outputPeak.load(), 1) + " dB",
               display.getX(), monitorY + monitorH + 3, display.getWidth(), 14,
               juce::Justification::centred);

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(8.0f));
    g.drawText("A/B = INDEPENDENT SETTINGS   |   SHIFT + A/B = STORE",
               cx, h - 50, cw, 13, juce::Justification::centred);

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
