void MaDriveAudioProcessorEditor::resized()
{
    const int w = getWidth();
    const int h = getHeight();
    const int sideW = juce::jlimit(230, 250, w / 4);
    const int leftX = 28;
    const int rightX = w - sideW - 28;
    const int knob = juce::jlimit(62, 68, (sideW - 18) / 3);

    auto K = [this, knob](int i, int x, int y)
    {
        knobs[i]->setBounds(x, y, knob, knob + 18);
        labels[i]->setBounds(x - 4, y + knob + 15, knob + 8, 16);
    };

    auto B = [this](int first, int count, int x, int y, int width, int height = 28)
    {
        const int gap = 4;
        const int bw = (width - gap * (count - 1)) / count;
        for (int i = 0; i < count; ++i)
            buttons[first + i]->setBounds(x + i * (bw + gap), y, bw, height);
    };

    // A/B: top-left, separated from the meter.
    buttons[14]->setBounds(leftX + 4, 92, 54, 30);
    buttons[15]->setBounds(leftX + 62, 92, 54, 30);

    // LEFT: Soft Clip, two rows.
    K(0, leftX + 2, 240);
    K(1, leftX + sideW / 2 + 1, 240);
    K(2, leftX + 2, 340);
    K(3, leftX + sideW / 2 + 1, 340);

    B(0, 3, leftX, 438, sideW, 27);
    B(3, 4, leftX, 471, sideW, 27);
    B(7, 3, leftX, 504, sideW, 27);

    // RIGHT: Drive.
    const int c1 = rightX + 1;
    const int c2 = rightX + sideW / 3;
    const int c3 = rightX + (sideW * 2) / 3 - 2;

    K(4, c1, 240);
    K(5, c2, 240);
    K(6, c3, 240);

    B(10, 3, rightX, 340, sideW, 27);

    K(7, c1, 380);
    K(8, c2, 380);
    K(9, c3, 380);

    K(10, c1, 480);
    K(11, c2, 480);

    // Bottom utilities.
    buttons[16]->setBounds(w / 2 - 88, h - 36, 176, 28);
    buttons[13]->setBounds(w - 118, h - 36, 98, 28);
}
