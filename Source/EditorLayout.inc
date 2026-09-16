void MaDriveAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    // 1. HEADER AREA (~45px)
    auto headerArea = bounds.removeFromTop(45);
    if (buttons.size() > 15)
    {
        auto abArea = headerArea.removeFromLeft(120).reduced(10, 8);
        buttons[14]->setBounds(abArea.removeFromLeft(45));
        abArea.removeFromLeft(5);
        buttons[15]->setBounds(abArea.removeFromLeft(45));
    }

    // 2. FOOTER AREA (~45px)
    auto footerArea = bounds.removeFromBottom(45);
    if (buttons.size() > 16)
        buttons[16]->setBounds(juce::Rectangle<int>(footerArea.getCentreX() - 75, footerArea.getY() + 8, 150, 28));

    if (knobs.size() > 11 && labels.size() > 11)
    {
        auto trimArea = footerArea.removeFromRight(90).reduced(2);
        labels[11]->setBounds(trimArea.removeFromTop(12));
        knobs[11]->setBounds(trimArea);
    }

    if (buttons.size() > 13)
        buttons[13]->setBounds(footerArea.removeFromRight(95).reduced(8, 8));

    // Helper Lambda internal untuk penataan Knob & Label
    auto placeKnob = [this](int index, juce::Rectangle<int> area)
    {
        if (knobs.size() > index && labels.size() > index)
        {
            labels[index]->setBounds(area.removeFromTop(14));
            knobs[index]->setBounds(area);
        }
    };

    // Helper Lambda internal untuk penataan Switch Button Group
    auto placeButtonGroup = [this](int startIndex, int count, juce::Rectangle<int> area)
    {
        if (count <= 0) return;
        int btnWidth = area.getWidth() / count;
        for (int i = 0; i < count; ++i)
        {
            if (buttons.size() > (startIndex + i))
                buttons[startIndex + i]->setBounds(area.removeFromLeft(btnWidth).reduced(2, 1));
        }
    };

    // 3. MAIN PANELS (Left, Center, Right)
    auto mainArea = bounds.reduced(10, 2);
    int panelWidth = mainArea.getWidth() / 3;

    auto leftPanel   = mainArea.removeFromLeft(panelWidth).reduced(4);
    auto rightPanel  = mainArea.removeFromRight(panelWidth).reduced(4);
    auto centerPanel = mainArea.reduced(4);

    // --- LEFT PANEL: SOFT CLIP ---
    leftPanel.removeFromTop(145); // Memaksimalkan space atas untuk VU Meter Besar

    auto leftUpper = leftPanel.removeFromTop(85);
    placeKnob(0, leftUpper.removeFromLeft(leftUpper.getWidth() / 2).reduced(2)); // INPUT
    placeKnob(1, leftUpper.reduced(2));                                            // THRESHOLD

    auto leftLower = leftPanel.removeFromTop(85);
    placeKnob(2, leftLower.removeFromLeft(leftLower.getWidth() / 2).reduced(2)); // KNEE
    placeKnob(3, leftLower.reduced(2));                                            // CLIP OUTPUT

    leftPanel.removeFromTop(6);
    placeButtonGroup(0, 3, leftPanel.removeFromTop(26)); // CLIP STYLE Switch
    leftPanel.removeFromTop(5);
    placeButtonGroup(3, 4, leftPanel.removeFromTop(26)); // OVERSAMPLING Switch
    leftPanel.removeFromTop(5);
    placeButtonGroup(7, 3, leftPanel.removeFromTop(26)); // PROCESS MODE Switch


    // --- RIGHT PANEL: DRIVE ---
    rightPanel.removeFromTop(145); // Memaksimalkan space atas untuk VU Meter Besar

    auto rightTop = rightPanel.removeFromTop(80);
    int topKnobW = rightTop.getWidth() / 3;
    placeKnob(4, rightTop.removeFromLeft(topKnobW).reduced(2)); // DRIVE
    placeKnob(5, rightTop.removeFromLeft(topKnobW).reduced(2)); // TONE
    placeKnob(6, rightTop.reduced(2));                          // DRIVE OUTPUT

    rightPanel.removeFromTop(5);
    placeButtonGroup(10, 3, rightPanel.removeFromTop(26)); // DRIVE MODE Switch (GROWL, ODD, TAPE)

    rightPanel.removeFromTop(8);
    auto rightMid = rightPanel.removeFromTop(80);
    placeKnob(7, rightMid.removeFromLeft(rightMid.getWidth() / 2).reduced(2)); // MIX
    placeKnob(8, rightMid.reduced(2));                                         // CHARACTER

    auto rightFilters = rightPanel.removeFromTop(80);
    placeKnob(9, rightFilters.removeFromLeft(rightFilters.getWidth() / 2).reduced(2)); // HPF
    placeKnob(10, rightFilters.reduced(2));                                             // LPF
}