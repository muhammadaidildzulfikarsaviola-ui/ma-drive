void MaDriveAudioProcessorEditor::paint(juce::Graphics& g)
{
    const int w = getWidth();
    const int h = getHeight();
    const int headerH = 116;
    const int footerH = 82;
    const auto orange = juce::Colour(0xffff8a2a);
    const auto metal = juce::Colour(0xffaaa49a);
    const auto black = juce::Colour(0xff141515);

    auto drawScrew = [&g](float x, float y)
    {
        g.setColour(juce::Colour(0xff373737));
        g.fillEllipse(x, y, 10.0f, 10.0f);
        g.setColour(juce::Colour(0xff111111));
        g.drawEllipse(x, y, 10.0f, 10.0f, 1.0f);
        g.drawLine(x + 2.0f, y + 5.0f, x + 8.0f, y + 5.0f, 1.0f);
    };

    auto drawPanel = [&g](juce::Rectangle<int> r)
    {
        g.setColour(juce::Colour(0xff151616));
        g.fillRoundedRectangle(r.toFloat(), 7.0f);
        g.setColour(juce::Colour(0xff6f6b64));
        g.fillRoundedRectangle(r.reduced(6).toFloat(), 4.0f);
        g.setColour(juce::Colour(0xff202020));
        g.drawRoundedRectangle(r.reduced(1).toFloat(), 7.0f, 2.0f);
    };

    g.fillAll(juce::Colour(0xff777168));
    auto chassis = getLocalBounds().reduced(8);
    g.setColour(juce::Colour(0xff1b1b1b));
    g.fillRoundedRectangle(chassis.toFloat(), 10.0f);
    g.setColour(metal);
    g.fillRoundedRectangle(chassis.reduced(7).toFloat(), 7.0f);

    g.setColour(juce::Colour(0x18222222));
    for (int y = 8; y < h; y += 7)
        g.drawHorizontalLine(y, 10.0f, (float)w - 10.0f);

    drawScrew(16, 12);
    drawScrew((float)w - 26, 12);
    drawScrew(16, (float)h - 22);
    drawScrew((float)w - 26, (float)h - 22);

    // HEADER
    auto header = juce::Rectangle<int>(24, 10, w - 48, headerH - 14);
    g.setColour(juce::Colour(0xffbdb6ab));
    g.fillRoundedRectangle(header.toFloat(), 6.0f);
    g.setColour(juce::Colour(0xff242424));
    g.drawRoundedRectangle(header.toFloat(), 6.0f, 2.0f);

    g.setColour(juce::Colour(0xff101010));
    g.setFont(juce::FontOptions(34.0f).withStyle("Bold"));
    g.drawText("WADIDAW", 0, 17, w, 42, juce::Justification::centred);
    g.setColour(orange);
    g.fillRect((float)w * 0.5f - 116.0f, 60.0f, 232.0f, 2.0f);
    g.setColour(juce::Colour(0xff202020));
    g.setFont(juce::FontOptions(10.0f).withStyle("Bold"));
    g.drawText("S A T U R A T I O N   /   C L I P P E R", 0, 67, w, 18, juce::Justification::centred);

    // MAIN THREE PANELS
    auto main = getLocalBounds().withTop(headerH).withBottom(h - footerH).reduced(22, 8);
    const int gap = 10;
    const int panelW = (main.getWidth() - gap * 2) / 3;
    auto left = main.removeFromLeft(panelW);
    main.removeFromLeft(gap);
    auto right = main.removeFromRight(panelW);
    main.removeFromRight(gap);
    auto center = main;

    drawPanel(left);
    drawPanel(center);
    drawPanel(right);

    auto panelTitle = [&g](juce::Rectangle<int> r, const char* title)
    {
        g.setColour(juce::Colour(0xff151515));
        g.setFont(juce::FontOptions(19.0f).withStyle("Bold"));
        g.drawText(title, r.getX() + 42, r.getY() + 10, r.getWidth() - 52, 22, juce::Justification::left);
        g.setColour(juce::Colour(0xffff8a2a));
        g.fillRoundedRectangle((float)r.getX() + 16.0f, (float)r.getY() + 13.0f, 17.0f, 17.0f, 3.0f);
        g.setColour(juce::Colour(0xffffc06b));
        g.fillEllipse((float)r.getX() + 20.0f, (float)r.getY() + 17.0f, 9.0f, 9.0f);
    };

    panelTitle(left, "SOFT CLIP");
    panelTitle(right, "DRIVE");

    auto drawVU = [&g](juce::Rectangle<int> r, float valueDb, const char* leftLabel, const char* rightLabel)
    {
        auto frame = r;
        g.setColour(juce::Colour(0xff101111));
        g.fillRoundedRectangle(frame.toFloat(), 7.0f);
        g.setColour(juce::Colour(0xff242424));
        g.drawRoundedRectangle(frame.toFloat(), 7.0f, 2.0f);
        auto face = frame.reduced(7, 7);
        juce::ColourGradient bg(juce::Colour(0xffffefbb), face.getX(), face.getY(),
                                juce::Colour(0xffffc240), face.getX(), face.getBottom(), false);
        g.setGradientFill(bg);
        g.fillRoundedRectangle(face.toFloat(), 4.0f);

        const float cx = face.getCentreX();
        const float cy = face.getBottom() + 9.0f;
        const float radius = juce::jmin(face.getWidth() * 0.40f, face.getHeight() * 0.98f);
        const float start = juce::MathConstants<float>::pi * 1.18f;
        const float end = juce::MathConstants<float>::pi * 1.82f;

        juce::Path arc;
        arc.addCentredArc(cx, cy, radius, radius, 0.0f, start, end, true);
        g.setColour(juce::Colour(0xff222222));
        g.strokePath(arc, juce::PathStrokeType(2.0f));

        const float tickDb[] = { -20, -10, -7, -5, -3, -1, 0, 1, 2, 3 };
        g.setFont(juce::FontOptions(8.0f).withStyle("Bold"));
        for (float db : tickDb)
        {
            const float t = juce::jmap(db, -20.0f, 3.0f, 0.0f, 1.0f);
            const float a = start + (end - start) * t;
            const float ri = radius - 2.0f;
            const float ro = radius - 12.0f;
            g.setColour(db >= 0.0f ? juce::Colour(0xffbb1f1f) : juce::Colour(0xff222222));
            g.drawLine(cx + std::cos(a) * ri, cy + std::sin(a) * ri,
                       cx + std::cos(a) * ro, cy + std::sin(a) * ro, 1.4f);
            g.drawText(juce::String((int)db),
                       (int)(cx + std::cos(a) * (radius - 30.0f)) - 15,
                       (int)(cy + std::sin(a) * (radius - 30.0f)) - 6,
                       30, 12, juce::Justification::centred);
        }

        g.setColour(juce::Colour(0xff111111));
        g.setFont(juce::FontOptions(17.0f).withStyle("Bold"));
        g.drawText("VU", face.getX(), face.getY() + 11, face.getWidth(), 24, juce::Justification::centred);

        const float t = juce::jlimit(0.0f, 1.0f, juce::jmap(valueDb, -20.0f, 3.0f, 0.0f, 1.0f));
        const float a = start + (end - start) * t;
        const float nx = cx + std::cos(a) * (radius - 8.0f);
        const float ny = cy + std::sin(a) * (radius - 8.0f);
        g.setColour(juce::Colour(0x30000000));
        g.drawLine(cx + 2.0f, cy + 2.0f, nx + 2.0f, ny + 2.0f, 4.0f);
        g.setColour(juce::Colour(0xffeeeeee));
        g.drawLine(cx, cy, nx, ny, 2.4f);
        g.setColour(juce::Colour(0xff242424));
        g.fillEllipse(cx - 7.0f, cy - 7.0f, 14.0f, 14.0f);

        g.setFont(juce::FontOptions(8.0f).withStyle("Bold"));
        g.drawText(leftLabel, face.getX() + 8, face.getBottom() - 17, 90, 12, juce::Justification::left);
        g.drawText(rightLabel, face.getRight() - 98, face.getBottom() - 17, 90, 12, juce::Justification::right);
    };

    drawVU({ left.getX() + 14, left.getY() + 46, left.getWidth() - 28, 142 }, p.inputPeak.load(), "INPUT", "THRESHOLD");
    drawVU({ right.getX() + 14, right.getY() + 46, right.getWidth() - 28, 142 }, p.outputPeak.load(), "INPUT", "OUTPUT");

    // CENTER SCREEN
    auto screen = juce::Rectangle<int>(center.getX() + 12, center.getY() + 42, center.getWidth() - 24, 258);
    g.setColour(juce::Colour(0xff080909));
    g.fillRoundedRectangle(screen.toFloat(), 5.0f);
    g.setColour(juce::Colour(0xff3a3a3a));
    g.drawRoundedRectangle(screen.toFloat(), 5.0f, 1.5f);

    const int processMode = (int)p.parameters.getRawParameterValue("processMode")->load();
    g.setColour(orange);
    g.setFont(juce::FontOptions(10.0f).withStyle("Bold"));
    g.drawText(processMode == 0 ? "WAVEFORM" : (processMode == 1 ? "M / S" : "MULTI BAND"),
               screen.getX() + 10, screen.getY() + 8, 105, 16, juce::Justification::left);
    g.setColour(juce::Colours::white);
    g.fillRect(screen.getCentreX() - 73, screen.getY() + 14, 24, 2);
    g.setColour(juce::Colour(0xffff3c32));
    g.fillRect(screen.getCentreX() + 9, screen.getY() + 14, 24, 2);
    g.setColour(juce::Colour(0xffededed));
    g.setFont(juce::FontOptions(8.0f).withStyle("Bold"));
    g.drawText("INPUT", screen.getCentreX() - 102, screen.getY() + 6, 48, 16, juce::Justification::left);
    g.drawText("OUTPUT", screen.getCentreX() + 38, screen.getY() + 6, 58, 16, juce::Justification::left);

    for (int i = 1; i < 7; ++i)
    {
        const int yy = screen.getY() + 34 + i * (screen.getHeight() - 46) / 7;
        g.setColour(juce::Colour(0xff242424));
        g.drawHorizontalLine(yy, (float)screen.getX(), (float)screen.getRight());
    }
    for (int i = 1; i < 7; ++i)
    {
        const int xx = screen.getX() + i * screen.getWidth() / 7;
        g.setColour(juce::Colour(0xff202020));
        g.drawVerticalLine(xx, (float)screen.getY() + 34.0f, (float)screen.getBottom());
    }

    juce::Path inputPath, outputPath;
    const int write = p.historyWrite.load();
    for (int i = 0; i < 256; ++i)
    {
        const int index = (write + i) & 255;
        const float inAmp = juce::jlimit(0.0f, 1.0f, std::abs(p.inputHistory[index]));
        const float outAmp = juce::jlimit(0.0f, 1.0f, std::abs(p.outputHistory[index]));
        const float x = screen.getX() + 8.0f + (screen.getWidth() - 16.0f) * i / 255.0f;
        const float base = screen.getY() + screen.getHeight() * 0.60f;
        const float yi = base - inAmp * screen.getHeight() * 0.38f;
        const float yo = base - outAmp * screen.getHeight() * 0.28f;
        if (i == 0) { inputPath.startNewSubPath(x, yi); outputPath.startNewSubPath(x, yo); }
        else { inputPath.lineTo(x, yi); outputPath.lineTo(x, yo); }
    }
    g.setColour(juce::Colours::white);
    g.strokePath(inputPath, juce::PathStrokeType(1.4f));
    g.setColour(juce::Colour(0xffff3330));
    g.strokePath(outputPath, juce::PathStrokeType(1.8f));

    const float threshold = juce::jlimit(-24.0f, 0.0f, p.thresholdForDisplay.load());
    const float thresholdNorm = juce::jlimit(0.0f, 1.0f, -threshold / 24.0f);
    const float thresholdY = screen.getY() + 36.0f + thresholdNorm * (screen.getHeight() - 48.0f);
    const float dashes[] = { 6.0f, 4.0f };
    g.setColour(orange);
    g.drawDashedLine({ (float)screen.getX() + 3.0f, thresholdY,
                       (float)screen.getRight() - 3.0f, thresholdY }, dashes, 2, 0.0f);
    g.setFont(juce::FontOptions(8.0f).withStyle("Bold"));
    g.drawText("THRESHOLD " + juce::String(threshold, 1) + " dB",
               screen.getX() + 8, (int)thresholdY - 13, 130, 11, juce::Justification::left);

    // CENTER MONITOR / LED METERS
    auto monitor = juce::Rectangle<int>(center.getX() + 20, screen.getBottom() + 12,
                                        center.getWidth() - 40, center.getBottom() - screen.getBottom() - 22);
    g.setColour(black);
    g.fillRoundedRectangle(monitor.toFloat(), 4.0f);
    g.setColour(juce::Colour(0xff3a3a3a));
    g.drawRoundedRectangle(monitor.toFloat(), 4.0f, 1.0f);

    g.setColour(juce::Colour(0xffeeeeee));
    g.setFont(juce::FontOptions(9.0f).withStyle("Bold"));
    g.drawText("IN", monitor.getX() + 12, monitor.getY() + 7, 36, 12, juce::Justification::centred);
    g.drawText("THRESHOLD MONITOR", monitor.getCentreX() - 75, monitor.getY() + 7, 150, 12, juce::Justification::centred);
    g.drawText("OUT", monitor.getRight() - 48, monitor.getY() + 7, 36, 12, juce::Justification::centred);

    auto drawLEDColumn = [&g](int x, int y, int width, int height, float db)
    {
        const int segs = 14;
        const int gapPx = 2;
        const int segH = juce::jmax(2, (height - gapPx * (segs - 1)) / segs);
        const float norm = juce::jlimit(0.0f, 1.0f, (db + 60.0f) / 60.0f);
        const int lit = (int)std::round(norm * segs);
        for (int i = 0; i < segs; ++i)
        {
            auto r = juce::Rectangle<int>(x, y + (segs - 1 - i) * (segH + gapPx), width, segH);
            if (i < lit)
            {
                const float t = i / (float)segs;
                g.setColour(t < 0.65f ? juce::Colour(0xff63d73f)
                                      : (t < 0.86f ? juce::Colour(0xffffc42b) : juce::Colour(0xfff23b2f)));
                g.fillRect(r);
            }
            else
            {
                g.setColour(juce::Colour(0xff262626));
                g.fillRect(r);
            }
        }
    };

    const int columnY = monitor.getY() + 27;
    const int columnH = juce::jmax(48, monitor.getHeight() - 34);
    drawLEDColumn(monitor.getX() + 16, columnY, 15, columnH, p.inputPeak.load());
    drawLEDColumn(monitor.getX() + 40, columnY, 15, columnH, p.inputPeak.load() - 1.0f);
    drawLEDColumn(monitor.getRight() - 55, columnY, 15, columnH, p.outputPeak.load());
    drawLEDColumn(monitor.getRight() - 31, columnY, 15, columnH, p.outputPeak.load() - 1.0f);

    const int grX = monitor.getCentreX() - 92;
    const int grY = monitor.getY() + 47;
    const int grW = 184;
    const int grH = 15;
    g.setColour(juce::Colour(0xff090909));
    g.fillRoundedRectangle((float)grX, (float)grY, (float)grW, (float)grH, 3.0f);
    g.setColour(juce::Colour(0xff414141));
    g.drawRoundedRectangle((float)grX, (float)grY, (float)grW, (float)grH, 3.0f, 1.0f);
    const float gr = juce::jlimit(0.0f, 1.0f, -p.reduction.load() / 24.0f);
    const int lit = (int)std::round(gr * 12.0f);
    for (int i = 0; i < 12; ++i)
    {
        auto cell = juce::Rectangle<int>(grX + 4 + i * 15, grY + 3, 11, 9);
        g.setColour(i < lit ? orange : juce::Colour(0xff242424));
        g.fillRect(cell);
    }
    g.setColour(juce::Colour(0xffa8a8a8));
    g.setFont(juce::FontOptions(7.0f));
    const char* grLabels[] = { "0", "-3", "-6", "-12", "-24" };
    const int gx[] = { grX - 2, grX + 39, grX + 79, grX + 121, grX + 161 };
    for (int i = 0; i < 5; ++i)
        g.drawText(grLabels[i], gx[i], grY + 20, 24, 10, juce::Justification::centred);

    // Footer visual hardware
    const int fy = h - footerH;
    g.setColour(juce::Colour(0xff171717));
    g.fillRoundedRectangle(22.0f, (float)fy, (float)w - 44.0f, 64.0f, 6.0f);
    g.setColour(juce::Colour(0xffbdb6ab));
    g.fillRoundedRectangle(28.0f, (float)fy + 6.0f, (float)w - 56.0f, 52.0f, 4.0f);
    g.setColour(juce::Colour(0xff151515));
    g.setFont(juce::FontOptions(20.0f).withStyle("Bold"));
    g.drawText("WADIDAW", 48, fy + 18, 150, 25, juce::Justification::left);
    g.setFont(juce::FontOptions(7.0f));
    g.drawText("SATURATION / CLIPPER", 49, fy + 40, 150, 10, juce::Justification::left);
    g.setColour(juce::Colour(0xff68635c));
    g.drawVerticalLine(w / 2 - 160, (float)fy + 12.0f, (float)fy + 52.0f);
    g.drawVerticalLine(w - 285, (float)fy + 12.0f, (float)fy + 52.0f);

    // Parameter-driven button styling
    const int clipStyle = (int)p.parameters.getRawParameterValue("clipStyle")->load();
    const int oversampling = (int)p.parameters.getRawParameterValue("oversampling")->load();
    const int processMode = (int)p.parameters.getRawParameterValue("processMode")->load();
    const int driveMode = (int)p.parameters.getRawParameterValue("driveMode")->load();
    for (int i = 0; i < 3; ++i)
        buttons[i]->setColour(juce::TextButton::buttonColourId, i == clipStyle ? orange : juce::Colour(0xff181818));
    for (int i = 0; i < 4; ++i)
        buttons[3 + i]->setColour(juce::TextButton::buttonColourId, i == oversampling ? orange : juce::Colour(0xff181818));
    for (int i = 0; i < 3; ++i)
        buttons[7 + i]->setColour(juce::TextButton::buttonColourId, i == processMode ? orange : juce::Colour(0xff181818));
    for (int i = 0; i < 3; ++i)
        buttons[10 + i]->setColour(juce::TextButton::buttonColourId, i == driveMode ? orange : juce::Colour(0xff181818));

    buttons[14]->setColour(juce::TextButton::buttonColourId, activeB ? juce::Colour(0xff181818) : orange);
    buttons[15]->setColour(juce::TextButton::buttonColourId, activeB ? orange : juce::Colour(0xff181818));
    const bool bypassed = p.parameters.getRawParameterValue("bypass")->load() > 0.5f;
    buttons[13]->setButtonText(bypassed ? "BYPASSED" : "BYPASS");
    buttons[13]->setColour(juce::TextButton::buttonColourId, bypassed ? orange : juce::Colour(0xff181818));

    // Live UI refresh for processor-owned meter data.
    if (isShowing())
        repaint();
}
