void MaDriveAudioProcessorEditor::resized()
{
    const int w = getWidth();
    const int h = getHeight();
    const int pw = juce::jlimit(215, 245, w / 4);
    const int lx = 28;
    const int rx = w - pw - 28;
    const int knob = juce::jlimit(72, 86, pw / 3);

    auto K = [this, knob](int i, int x, int y)
    {
        knobs[i]->setBounds(x, y, knob, knob + 20);
        labels[i]->setBounds(x - 4, y + knob + 15, knob + 8, 16);
    };

    auto B = [this](int first, int count, int x, int y, int width, int height = 29)
    {
        const int gap = 4;
        const int bw = (width - gap * (count - 1)) / count;
        for (int i = 0; i < count; ++i)
            buttons[first + i]->setBounds(x + i * (bw + gap), y, bw, height);
    };

    // A/B at the top-left.
    buttons[14]->setBounds(lx + 14, 82, 55, 30);
    buttons[15]->setBounds(lx + 73, 82, 55, 30);

    // LEFT: Soft Clip controls.
    K(0, lx + 2, 245);
    K(1, lx + pw / 2 + 2, 245);
    K(2, lx + 2, 335);
    K(3, lx + pw / 2 + 2, 335);
    B(0, 3, lx, 438, pw, 28);
    B(3, 4, lx, 471, pw, 28);
    B(7, 3, lx, 504, pw, 28);

    // RIGHT: Drive controls.
    K(4, rx + 1, 245);
    K(5, rx + pw / 2 - knob / 2, 245);
    K(6, rx + pw - knob - 1, 245);
    B(10, 3, rx, 337, pw, 28);
    K(7, rx + 1, 350);
    K(8, rx + pw / 2 - knob / 2, 350);
    K(9, rx + pw - knob - 1, 350);
    K(10, rx + 1, 435);
    K(11, rx + pw / 2 - knob / 2, 435);

    // Global controls.
    buttons[13]->setBounds(w - 105, h - 34, 88, 28);
    buttons[16]->setBounds(w / 2 - 70, h - 34, 140, 28);
}
