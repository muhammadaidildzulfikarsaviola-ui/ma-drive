MaDriveAudioProcessorEditor::MaDriveAudioProcessorEditor(MaDriveAudioProcessor& x)
    : AudioProcessorEditor(&x), p(x)
{
    setLookAndFeel(&hardwareLookAndFeel);

    const char* ids[] = { "input", "threshold", "knee", "clipOutput", "drive", "tone", "driveOutput", "mix", "character", "hpf", "lpf", "trim" };
    const char* names[] = { "INPUT", "THRESHOLD", "KNEE", "OUTPUT", "DRIVE", "TONE", "OUTPUT", "MIX", "CHARACTER", "HPF", "LPF", "TRIM" };

    for (int i = 0; i < 12; ++i)
    {
        auto* s = knobs.add(new juce::Slider(juce::Slider::RotaryHorizontalVerticalDrag,
                                              juce::Slider::TextBoxBelow));
        s->setLookAndFeel(&hardwareLookAndFeel);
        s->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 76, 18);
        s->setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        s->setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff45433f));
        s->setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff77736b));
        s->setNumDecimalPlacesToDisplay((i == 7 || i == 8) ? 0 : 2);

        float start = juce::MathConstants<float>::pi * 0.75f;
        float end   = juce::MathConstants<float>::pi * 2.25f;

        // Bipolar controls: minimum around 7:30, 0 at 12, maximum around 4:30.
        if (i == 0 || i == 2 || i == 3 || i == 6 || i == 11 || i == 5)
        {
            start = juce::MathConstants<float>::pi * 0.75f;
            end   = juce::MathConstants<float>::pi * 2.25f;
        }
        // Threshold: minimum around 7:30, 0 dB at 5 o'clock.
        else if (i == 1)
        {
            start = juce::MathConstants<float>::pi * 0.75f;
            end   = juce::MathConstants<float>::pi * 2.3333333f;
        }
        // Drive / Mix / Character: 0 at 7 o'clock, maximum at 5 o'clock.
        else if (i == 4 || i == 7 || i == 8)
        {
            start = juce::MathConstants<float>::pi * 0.6666667f;
            end   = juce::MathConstants<float>::pi * 2.3333333f;
        }
        // HPF: OFF at 7 o'clock, increasing frequency clockwise toward 5 o'clock.
        else if (i == 9)
        {
            start = juce::MathConstants<float>::pi * 0.6666667f;
            end   = juce::MathConstants<float>::pi * 2.3333333f;
        }
        // LPF: OFF at 5:30, 20 kHz at 5 o'clock, sweep counter-clockwise.
        else if (i == 10)
        {
            start = juce::MathConstants<float>::pi * 0.4166667f;
            end   = juce::MathConstants<float>::pi * 0.25f;
        }

        s->setRotaryParameters(start, end, true);

        if (i == 7 || i == 8)
            s->setTextValueSuffix(" %");
        else if (i == 9)
            s->textFromValueFunction = [](double v) { return v <= 0.001 ? juce::String("OFF") : juce::String(v, 1) + " Hz"; };
        else if (i == 10)
            s->textFromValueFunction = [](double v) { return v <= 0.001 ? juce::String("OFF") : juce::String(v, 0) + " Hz"; };
        else if (i == 5)
            s->setTextValueSuffix("");
        else
            s->setTextValueSuffix(" dB");

        addAndMakeVisible(s);
        attachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            p.parameters, ids[i], *s));

        auto* l = labels.add(new juce::Label({}, names[i]));
        l->setJustificationType(juce::Justification::centred);
        l->setColour(juce::Label::textColourId, juce::Colours::black);
        l->setFont(juce::FontOptions(11.0f).withStyle("Bold"));
        addAndMakeVisible(l);
    }

    const char* buttonNames[] = {
        "SOFT", "MEDIUM", "HARD",
        "1x", "2x", "4x", "8x",
        "STEREO", "M/S", "MULTI BAND",
        "GROWL", "ODD", "TAPE",
        "BYPASS", "A", "B", "SUPPORT ME"
    };

    for (const auto* text : buttonNames)
    {
        auto* b = buttons.add(new juce::TextButton(text));
        b->setColour(juce::TextButton::buttonColourId, juce::Colour(0xff151616));
        b->setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xffff8a2a));
        b->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        b->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
        b->setClickingTogglesState(false);
        addAndMakeVisible(b);
    }

    auto makeChoiceButtons = [this](const char* parameterId, int first, int count)
    {
        for (int i = 0; i < count; ++i)
        {
            buttons[first + i]->onClick = [this, parameterId, i]
            {
                if (auto* parameter = dynamic_cast<juce::AudioParameterChoice*>(p.parameters.getParameter(parameterId)))
                    parameter->setValueNotifyingHost(parameter->convertTo0to1(i));
            };
        }
    };

    makeChoiceButtons("clipStyle", 0, 3);
    makeChoiceButtons("oversampling", 3, 4);
    makeChoiceButtons("processMode", 7, 3);
    makeChoiceButtons("driveMode", 10, 3);

    buttons[13]->setClickingTogglesState(true);
    buttonAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        p.parameters, "bypass", *buttons[13]));

    buttons[14]->onClick = [this]
    {
        if (juce::ModifierKeys::getCurrentModifiers().isShiftDown())
            p.copyAB(false);
        else
        {
            p.recallAB(false);
            activeB = false;
        }
    };

    buttons[15]->onClick = [this]
    {
        if (juce::ModifierKeys::getCurrentModifiers().isShiftDown())
            p.copyAB(true);
        else
        {
            p.recallAB(true);
            activeB = true;
        }
    };

    buttons[16]->onClick = []
    {
        juce::URL("https://wadidaw.com").launchInDefaultBrowser();
    };

    setSize(900, 550);
    setResizable(true, true);
    setResizeLimits(900, 550, 1500, 900);
    startTimerHz(30);
}

MaDriveAudioProcessorEditor::~MaDriveAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
    for (auto* s : knobs)
        s->setLookAndFeel(nullptr);
}
