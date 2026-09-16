void MaDriveAudioProcessorEditor::paint(juce::Graphics& g)
{
    const int w = getWidth();
    const int h = getHeight();
    const int pw = juce::jlimit(230, 245, w / 4);
    const int lx = 28;
    const int rx = w - pw - 28;
    const int cx = lx + pw + 18;
    const int cw = juce::jmax(260, rx - cx - 18);
    const int footerY = h - 78;

    // Industrial metal base.
    g.fillAll(juce::Colour(0xff777168));
    auto outer = getLocalBounds().reduced(10);
    g.setColour(juce::Colour(0xff171818));
    g.fillRoundedRectangle(outer.toFloat(), 12.0f);
    g.setColour(juce::Colour(0xffaaa49a));
    g.fillRoundedRectangle(outer.reduced(8).toFloat(), 8.0f);

    // Header.
    g.setColour(juce::Colour(0xff1a1a19));
    g.fillRoundedRectangle(22.0f, 10.0f, (float)w - 44.0f, 110.0f, 8.0f);
    g.setColour(juce::Colour(0xffc4beb2));
    g.fillRoundedRectangle(28.0f, 16.0f, (float)w - 56.0f, 98.0f, 6.0f);

    g.setColour(juce::Colour(0xff111212));
    g.setFont(juce::FontOptions(34.0f).withStyle("Bold"));
    g.drawText("WADIDAW", 0, 18, w, 42, juce::Justification::centred);
    g.setColour(juce::Colour(0xff222222));
    g.setFont(juce::FontOptions(11.0f).withStyle("Bold"));
    g.drawText("S A T U R A T I O N   /   C L I P P E R", 0, 64, w, 18, juce::Justification::centred);

    // Three main chassis sections.
    auto drawPanel = [&](int x, int y, int ww, int hh)
    {
        g.setColour(juce::Colour(0xff151616));
        g.fillRoundedRectangle((float)x, (float)y, (float)ww, (float)hh, 9.0f);
        g.setColour(juce::Colour(0xff7d786f));
        g.fillRoundedRectangle((float)x + 7.0f, (float)y + 7.0f,
                               (float)ww - 14.0f, (float)hh - 14.0f, 6.0f);
    };

    drawPanel(lx - 10, 126, pw + 20, footerY - 136);
    drawPanel(cx - 8, 126, cw + 16, footerY - 136);
    drawPanel(rx - 10, 126, pw + 20, footerY - 136);

    g.setColour(juce::Colour(0xff111111));
    g.setFont(juce::FontOptions(21.0f).withStyle("Bold"));
    g.drawText("SOFT CLIP", lx, 138, pw, 28, juce::Justification::centred);
    g.drawText("DRIVE", rx, 138, pw, 28, juce::Justification::centred);

    // Single dedicated GR VU meter in the centre. Only the number of needles changes.
    const int meterX = cx + 10;
    const int meterY = 166;
    const int meterW = cw - 20;
    const int meterH = 132;
    g.setColour(juce::Colour(0xff171818));
    g.fillRoundedRectangle((float)meterX, (float)meterY, (float)meterW, (float)meterH, 7.0f);
    g.setColour(juce::Colour(0xffe1c86c));
    g.fillRoundedRectangle((float)meterX + 6.0f, (float)meterY + 6.0f,
                           (float)meterW - 12.0f, (float)meterH - 12.0f, 5.0f);

    const float centreX = meterX + meterW * 0.5f;
    const float centreY = meterY + meterH * 0.82f;
    const float radius = juce::jmin(meterW * 0.39f, meterH * 0.53f);
    const float a0 = juce::MathConstants<float>::pi * 1.18f;
    const float a1 = juce::MathConstants<float>::pi * 1.82f;

    g.setColour(juce::Colour(0xff222222));
    juce::Path arc;
    arc.addCentredArc(centreX, centreY, radius, radius, 0.0f, a0, a1, true);
    g.strokePath(arc, juce::PathStrokeType(2.0f));

    const float scale[] = { -24.0f, -18.0f, -12.0f, -6.0f, 0.0f };
    for (int i = 0; i < 5; ++i)
    {
        const float t = i / 4.0f;
        const float a = a0 + (a1 - a0) * t;
        const float ri = radius - 2.0f;
        const float ro = radius - 11.0f;
        g.setColour(juce::Colour(0xff252525));
        g.drawLine(centreX + std::cos(a) * ri, centreY + std::sin(a) * ri,
                   centreX + std::cos(a) * ro, centreY + std::sin(a) * ro, 1.2f);
        g.setFont(juce::FontOptions(7.0f).withStyle("Bold"));
        g.drawText(juce::String(scale[i], 0),
                   (int)(centreX + std::cos(a) * (radius - 23.0f)) - 14,
                   (int)(centreY + std::sin(a) * (radius - 23.0f)) - 5,
                   28, 10, juce::Justification::centred);
    }

    const int mode = (int)p.parameters.getRawParameterValue("processMode")->load();
    const int needleCount = mode == 0 ? 1 : (mode == 1 ? 2 : 3);
    const float threshold = juce::jlimit(-24.0f, 0.0f, p.thresholdForDisplay.load());

    g.setColour(juce::Colour(0xff111111));
    g.setFont(juce::FontOptions(13.0f).withStyle("Bold"));
    g.drawText("GAIN REDUCTION", meterX, meterY + 7, meterW, 16, juce::Justification::centred);

    for (int i = 0; i < needleCount; ++i)
    {
        float grDb = p.reduction.load();
        if (mode != 0)
        {
            const float sourceDb = p.monitorPeaks[i].load();
            grDb = -juce::jmax(0.0f, sourceDb - threshold);
        }
        grDb = juce::jlimit(-24.0f, 0.0f, grDb);
        const float t = juce::jlimit(0.0f, 1.0f, -grDb / 24.0f);
        const float a = a1 - (a1 - a0) * t;
        const float pivotX = centreX + (needleCount == 1 ? 0.0f : (i - (needleCount - 1) * 0.5f) * 20.0f);
        const float length = radius - 18.0f;
        g.setColour(juce::Colour(0xffff5b32));
        g.drawLine(pivotX, centreY,
                   pivotX + std::cos(a) * length,
                   centreY + std::sin(a) * length, 2.4f);
        g.fillEllipse(pivotX - 3.0f, centreY - 3.0f, 6.0f, 6.0f);

        const juce::String label = mode == 0 ? "ST" :
                                   (mode == 1 ? (i == 0 ? "M" : "S") :
                                    (i == 0 ? "LOW" : (i == 1 ? "MID" : "HIGH")));
        g.setColour(juce::Colour(0xff222222));
        g.setFont(juce::FontOptions(7.0f).withStyle("Bold"));
        g.drawText(label, (int)pivotX - 22, meterY + meterH - 18, 44, 10, juce::Justification::centred);
    }

    // Centre signal field.
    const int displayX = cx + 10;
    const int displayY = 312;
    const int displayW = cw - 20;
    const int displayH = juce::jmax(150, footerY - displayY - 18);
    g.setColour(juce::Colour(0xff080909));
    g.fillRoundedRectangle((float)displayX, (float)displayY, (float)displayW, (float)displayH, 5.0f);

    // Field header with realtime signal values.
    g.setColour(juce::Colour(0xffff8a2a));
    g.setFont(juce::FontOptions(9.0f).withStyle("Bold"));
    g.drawText("INPUT  " + juce::String(p.inputPeak.load(), 1) + " dB",
               displayX + 8, displayY + 8, displayW / 3 - 8, 15, juce::Justification::left);
    g.drawText("GR  " + juce::String(p.reduction.load(), 1) + " dB",
               displayX + displayW / 3, displayY + 8, displayW / 3, 15, juce::Justification::centred);
    g.drawText("OUTPUT  " + juce::String(p.outputPeak.load(), 1) + " dB",
               displayX + 2 * displayW / 3, displayY + 8, displayW / 3 - 8, 15, juce::Justification::right);

    g.setColour(juce::Colour(0xff343434));
    for (int i = 1; i < 7; ++i)
        g.drawHorizontalLine(displayY + 30 + i * (displayH - 38) / 7,
                             (float)displayX, (float)(displayX + displayW));
    for (int i = 1; i < 5; ++i)
        g.drawVerticalLine(displayX + i * displayW / 5,
                           (float)displayY + 25.0f, (float)(displayY + displayH));

    // dB scale at left of the signal field.
    g.setColour(juce::Colour(0xff999999));
    g.setFont(juce::FontOptions(7.0f));
    const float dbScale[] = { 0.0f, -6.0f, -12.0f, -18.0f, -24.0f };
    for (int i = 0; i < 5; ++i)
        g.drawText(juce::String(dbScale[i], 0), displayX - 28,
                   displayY + 34 + i * (displayH - 48) / 4,
                   25, 10, juce::Justification::right);

    // Threshold guide: 0 dB at the top, more negative values lower down.
    const float thresholdT = juce::jlimit(0.0f, 1.0f, -threshold / 24.0f);
    const float thresholdY = displayY + 28.0f + thresholdT * (displayH - 42.0f);
    const float dashPattern[] = { 6.0f, 4.0f };
    g.setColour(juce::Colour(0xffff8a2a));
    g.drawDashedLine(juce::Line<float>((float)displayX, thresholdY,
                                       (float)(displayX + displayW), thresholdY),
                     dashPattern, 2, 0.0f);
    g.setFont(juce::FontOptions(7.0f).withStyle("Bold"));
    g.drawText("THRESHOLD " + juce::String(threshold, 1) + " dB",
               displayX + 8, (int)thresholdY - 15, 130, 11, juce::Justification::left);

    // Live waveform from the audio history buffer.
    juce::Path inputPath, outputPath;
    const int write = p.historyWrite.load();
    for (int i = 0; i < 256; ++i)
    {
        const int index = (write + i) & 255;
        const float inAmp = juce::jlimit(0.0f, 1.0f, std::abs(p.inputHistory[index]));
        const float outAmp = juce::jlimit(0.0f, 1.0f, std::abs(p.outputHistory[index]));
        const float xx = displayX + displayW * i / 255.0f;
        const float yyIn = displayY + displayH * 0.56f - inAmp * displayH * 0.42f;
        const float yyOut = displayY + displayH * 0.56f - outAmp * displayH * 0.30f;
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
    g.strokePath(inputPath, juce::PathStrokeType(1.3f));
    g.setColour(juce::Colour(0xfff3372f));
    g.strokePath(outputPath, juce::PathStrokeType(1.7f));

    g.setColour(juce::Colour(0xff777777));
    g.setFont(juce::FontOptions(7.0f).withStyle("Bold"));
    g.drawText(mode == 0 ? "STEREO SIGNAL" : (mode == 1 ? "M / S SIGNAL" : "MULTI BAND SIGNAL"),
               displayX + 8, displayY + displayH - 15, displayW - 16, 10, juce::Justification::centred);

    // Left controls.
    g.setColour(juce::Colour(0xff222222));
    g.setFont(juce::FontOptions(9.0f).withStyle("Bold"));
    g.drawText("CLIP STYLE", lx, 382, pw, 12, juce::Justification::left);
    g.drawText("OVERSAMPLING", lx, 416, pw, 12, juce::Justification::left);
    g.drawText("PROCESS MODE", lx, 450, pw, 12, juce::Justification::left);

    // Right controls.
    g.drawText("DRIVE MODE", rx, 292, pw, 12, juce::Justification::left);
    g.drawText("DRIVE FILTERS", rx, 374, pw, 12, juce::Justification::left);

    // Footer frame and branding.
    g.setColour(juce::Colour(0xff171818));
    g.fillRoundedRectangle(22.0f, (float)footerY, (float)w - 44.0f, 66.0f, 8.0f);
    g.setColour(juce::Colour(0xffaaa49a));
    g.fillRoundedRectangle(28.0f, (float)footerY + 6.0f, (float)w - 56.0f, 54.0f, 5.0f);
    g.setColour(juce::Colour(0xff202020));
    g.setFont(juce::FontOptions(20.0f).withStyle("Bold"));
    g.drawText("WADIDAW", 44, footerY + 20, 150, 25, juce::Justification::left);
    g.setFont(juce::FontOptions(7.0f));
    g.drawText("SATURATION / CLIPPER", 44, footerY + 43, 150, 10, juce::Justification::left);

    // Footer separators.
    g.setColour(juce::Colour(0xff68635c));
    g.drawVerticalLine(w / 2 - 150, (float)footerY + 12.0f, (float)footerY + 54.0f);
    g.drawVerticalLine(w - 300, (float)footerY + 12.0f, (float)footerY + 54.0f);

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
