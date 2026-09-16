void MaDriveAudioProcessorEditor::resized()
{
    const int w = getWidth();
    const int h = getHeight();
    const int sideW = juce::jlimit(230, 245, w / 4);
    const int leftX = 28;
    const int rightX = w - sideW - 28;
    const int knob = juce::jlimit(62, 68, (sideW - 18) / 3);
    const int footerY = h - 78;

    auto K = [this, knob](int i, int x, int y)
    {
        knobs[i]->setBounds(x, y, knob, knob + 18);
        labels[i]->setBounds(x - 5, y + knob + 12, knob + 10, 16);
    };

    auto KS = [this](int i, int x, int y, int size)
    {
        knobs[i]->setBounds(x, y, size, size + 16);
        labels[i]->setBounds(x - 5, y + size + 11, size + 10, 15);
    };

    auto B = [this](int first, int count, int x, int y, int width, int height = 27)
    {
        const int gap = 4;
        const int bw = (width - gap * (count - 1)) / count;
        for (int i = 0; i < count; ++i)
            buttons[first + i]->setBounds(x + i * (bw + gap), y, bw, height);
    };

    // Header A/B strip.
    buttons[14]->setBounds(42, 28, 48, 30);
    buttons[15]->setBounds(92, 28, 48, 30);

    // LEFT / SOFT CLIP.
    K(0, leftX + 4, 205);
    K(1, leftX + sideW / 2 + 2, 205);
    K(2, leftX + 4, 300);
    K(3, leftX + sideW / 2 + 2, 300);
    B(0, 3, leftX, 390, sideW, 27);
    B(3, 4, leftX, 424, sideW, 27);
    B(7, 3, leftX, 458, sideW, 27);

    // RIGHT / DRIVE.
    const int c1 = rightX + 2;
    const int c2 = rightX + sideW / 3;
    const int c3 = rightX + (sideW * 2) / 3 - 2;
    K(4, c1, 205);
    K(5, c2, 205);
    K(6, c3, 205);
    B(10, 3, rightX, 300, sideW, 27);
    K(7, c1, 315);
    K(8, c3, 315);
    K(9, c1, 385);
    K(10, c2, 385);

    // FOOTER: TRIM + SUPPORT ME + BYPASS.
    KS(11, w - 135, footerY - 2, 54);
    buttons[16]->setBounds(w / 2 - 95, footerY + 23, 190, 30);
    buttons[13]->setBounds(w - 270, footerY + 23, 110, 30);
}
