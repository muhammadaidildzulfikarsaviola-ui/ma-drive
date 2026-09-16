void MaDriveAudioProcessorEditor::resized()
{
    const int w = getWidth();
    const int h = getHeight();

    auto bounds = getLocalBounds();

    // Header / top chassis
    auto header = bounds.removeFromTop(116).reduced(28, 12);
    buttons[14]->setBounds(header.getX(), header.getY() + 22, 44, 32);
    buttons[15]->setBounds(header.getX() + 48, header.getY() + 22, 44, 32);

    // Footer / bottom chassis
    auto footer = bounds.removeFromBottom(82).reduced(28, 10);
    buttons[16]->setBounds(footer.getCentreX() - 95, footer.getY() + 14, 190, 34);
    buttons[13]->setBounds(footer.getRight() - 124, footer.getY() + 16, 104, 32);
    labels[11]->setBounds(footer.getRight() - 215, footer.getY() + 12, 78, 14);
    knobs[11]->setBounds(footer.getRight() - 207, footer.getY() + 27, 62, 48);

    // Main three-section hardware layout
    auto main = bounds.reduced(18, 8);
    const int gap = 12;
    const int leftW = juce::jmax(270, (main.getWidth() - gap * 2) / 3);
    const int rightW = leftW;

    auto left = main.removeFromLeft(leftW);
    main.removeFromLeft(gap);
    auto right = main.removeFromRight(rightW);
    main.removeFromRight(gap);
    auto center = main;

    auto placeKnob = [this](int index, juce::Rectangle<int> area)
    {
        const int labelH = 16;
        labels[index]->setBounds(area.removeFromTop(labelH));
        knobs[index]->setBounds(area.reduced(4, 0));
    };

    auto placeRow = [this](int start, int count, juce::Rectangle<int> area)
    {
        const int gapPx = 6;
        const int totalGap = gapPx * (count - 1);
        const int bw = juce::jmax(42, (area.getWidth() - totalGap) / count);
        for (int i = 0; i < count; ++i)
        {
            auto b = area.removeFromLeft(bw);
            buttons[start + i]->setBounds(b);
            if (i < count - 1)
                area.removeFromLeft(gapPx);
        }
    };

    // LEFT / SOFT CLIP
    auto leftContent = left.reduced(14, 12);
    leftContent.removeFromTop(144); // dedicated painted VU meter area

    auto row1 = leftContent.removeFromTop(92);
    placeKnob(0, row1.removeFromLeft(row1.getWidth() / 2).reduced(2, 0));
    placeKnob(1, row1.reduced(2, 0));

    leftContent.removeFromTop(6);
    auto row2 = leftContent.removeFromTop(92);
    placeKnob(2, row2.removeFromLeft(row2.getWidth() / 2).reduced(2, 0));
    placeKnob(3, row2.reduced(2, 0));

    leftContent.removeFromTop(10);
    placeRow(0, 3, leftContent.removeFromTop(31));
    leftContent.removeFromTop(8);
    placeRow(3, 4, leftContent.removeFromTop(31));
    leftContent.removeFromTop(8);
    placeRow(7, 3, leftContent.removeFromTop(31));

    // RIGHT / DRIVE
    auto rightContent = right.reduced(14, 12);
    rightContent.removeFromTop(144); // dedicated painted VU meter area

    auto driveRow = rightContent.removeFromTop(92);
    const int third = driveRow.getWidth() / 3;
    placeKnob(4, driveRow.removeFromLeft(third).reduced(2, 0));
    placeKnob(5, driveRow.removeFromLeft(third).reduced(2, 0));
    placeKnob(6, driveRow.reduced(2, 0));

    rightContent.removeFromTop(8);
    placeRow(10, 3, rightContent.removeFromTop(31));
    rightContent.removeFromTop(10);

    auto mixRow = rightContent.removeFromTop(92);
    placeKnob(7, mixRow.removeFromLeft(mixRow.getWidth() / 2).reduced(2, 0));
    placeKnob(8, mixRow.reduced(2, 0));

    rightContent.removeFromTop(8);
    auto filterRow = rightContent.removeFromTop(92);
    placeKnob(9, filterRow.removeFromLeft(filterRow.getWidth() / 2).reduced(2, 0));
    placeKnob(10, filterRow.reduced(2, 0));

    juce::ignoreUnused(w, h, center);
}
