void MaDriveAudioProcessorEditor::resized()
{
    const int w = getWidth();
    const int h = getHeight();
    const int sideW = juce::jlimit(230, 245, w / 4);
    const int leftX = 28;
    const int rightX = w - sideW - 28;
    const int knob = juce::jlimit(62, 68, (sideW - 18) / 3);

    auto K = [this, knob](int i, int x, int y)
    {
        knobs[i]->setBounds(x, y, knob, knob + 18);
        labels[i]->setBounds(x - 4, y + knob + 15, knob + 8, 16);
    };

    auto KS = [this](int i, int x, int y, int size)
    {
        knobs[i]->setBounds(x, y, size, size + 16);
        labels[i]->setBounds(x - 5, y + size + 12, size + 10, 15);
    };

    auto B = [this](int first, int count, int x, int y, int width, int height = 27)
    {
        const int gap = 4;
        const int bw = (width - gap * (count - 1)) / count;
        for (int i = 0; i < count; ++i)
            buttons[first + i]->setBounds(x + i * (bw + gap), y, bw, height);
    };

    // A/B: dedicated strip above the meters. No overlap.
    buttons[14]->setBounds(leftX + 4, 82, 54, 29);
    buttons[15]->setBounds(leftX + 62, 82, 54, 29);

    // LEFT: Soft Clip.
    K(0, leftX + 2, 235);
    K(1, leftX + sideW / 2 + 1, 235);
    K(2, leftX + 2, 325);
    K(3, leftX + sideW / 2 + 1, 325);
    B(0, 3, leftX, 408, sideW, 26);
    B(3, 4, leftX, 440, sideW, 26);
    B(7, 3, leftX, 472, sideW, 26);

    // RIGHT: Drive.
    const int c1 = rightX + 1;
    const int c2 = rightX + sideW / 3;
    const int c3 = rightX + (sideW * 2) / 3 - 2;

    K(4, c1, 235);
    K(5, c2, 235);
    K(6, c3, 235);
    B(10, 3, rightX, 322, sideW, 26);
    K(7, c1, 360);
    K(8, c2, 360);
    K(9, c3, 360);

    // Bottom utility footer: LPF + Trim live with Support + Bypass.
    const int footerY = h - 91;
    const int small = 54;
    KS(10, rightX + 5, footerY + 2, small);
    KS(11, rightX + 67, footerY + 2, small);

    buttons[16]->setBounds(w / 2 - 90, h - 34, 180, 28);
    buttons[13]->setBounds(w - 118, h - 34, 98, 28);
}
