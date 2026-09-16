#include "PluginEditor.h"
namespace { juce::Colour metal(0xff777168), dark(0xff121313), amber(0xffff8a2a), red(0xfff3372f); }
MaDriveAudioProcessorEditor::MaDriveAudioProcessorEditor(MaDriveAudioProcessor& x):AudioProcessorEditor(&x),p(x){const char* ids[]={"input","threshold","knee","clipOutput","drive","tone","driveOutput","mix","character","hpf","lpf","trim"};const char* names[]={"INPUT","THRESHOLD","KNEE","OUTPUT","DRIVE","TONE","OUTPUT","MIX","CHARACTER","HPF","LPF","TRIM"};for(int i=0;i<12;++i){auto*s=knobs.add(new juce::Slider(juce::Slider::RotaryHorizontalVerticalDrag,juce::Slider::TextBoxBelow));s->setColour(juce::Slider::rotarySliderFillColourId,amber);s->setColour(juce::Slider::thumbColourId,juce::Colours::white);addAndMakeVisible(s);attachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,ids[i],*s));auto*l=labels.add(new juce::Label({},names[i]));l->setJustificationType(juce::Justification::centred);l->setColour(juce::Label::textColourId,juce::Colours::black);l->setFont(juce::FontOptions(14).withStyle("Bold"));addAndMakeVisible(l);} const char* bs[]={"SOFT","MEDIUM","HARD","1x","2x","4x","8x","STEREO","M/S","MULTI BAND","GROWL","ODD","TAPE","BYPASS","A","B","SUPPORT ME"};for(auto n:bs){auto*b=buttons.add(new juce::TextButton(n));b->setColour(juce::TextButton::buttonColourId,dark);b->setColour(juce::TextButton::textColourOffId,juce::Colours::whitesmoke);b->setColour(juce::TextButton::buttonOnColourId,amber);addAndMakeVisible(b);} auto select=[this](const char* id,int first,int count){for(int i=0;i<count;++i){buttons[first+i]->setRadioGroupId(first+1);buttons[first+i]->setClickingTogglesState(true);buttons[first+i]->onClick=[this,id,i]{if(auto* q=dynamic_cast<juce::AudioParameterChoice*>(p.parameters.getParameter(id)))q->setValueNotifyingHost(q->convertTo0to1(i));};}};select("clipStyle",0,3);select("oversampling",3,4);select("processMode",7,3);select("driveMode",10,3);buttonAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.parameters,"bypass",*buttons[13]));buttons[14]->onClick=[this]{p.copyAB(false);p.recallAB(false);};buttons[15]->onClick=[this]{p.copyAB(true);p.recallAB(true);};buttons[16]->onClick=[] { juce::URL("https://wadidaw.com").launchInDefaultBrowser(); };setSize(1280,820);setResizable(true,true);startTimerHz(30);}
MaDriveAudioProcessorEditor::~MaDriveAudioProcessorEditor(){stopTimer();}
void MaDriveAudioProcessorEditor::resized()
{
    const auto width = getWidth();
    const auto leftX = 38;
    const auto panelWidth = 280;
    const auto rightX = width - leftX - panelWidth;
    const auto knobSize = 88;

    auto setKnob = [this, knobSize] (int index, int x, int y)
    {
        knobs[index]->setBounds (x, y, knobSize, knobSize);
        labels[index]->setBounds (x - 8, y + 82, knobSize + 16, 20);
    };

    buttons[14]->setBounds (52, 47, 48, 34);
    buttons[15]->setBounds (104, 47, 48, 34);

    setKnob (0, leftX + 28, 266);  setKnob (1, leftX + 164, 266);
    setKnob (2, leftX + 28, 382);  setKnob (3, leftX + 164, 382);

    auto setButtonRow = [this] (int first, int count, int x, int y, int totalWidth)
    {
        const auto gap = 4;
        const auto buttonWidth = (totalWidth - gap * (count - 1)) / count;
        for (int i = 0; i < count; ++i)
            buttons[first + i]->setBounds (x + i * (buttonWidth + gap), y, buttonWidth, 34);
    };

    setButtonRow (0, 3, leftX + 16, 530, panelWidth - 32);
    setButtonRow (3, 4, leftX + 16, 570, panelWidth - 32);
    setButtonRow (7, 3, leftX + 16, 610, panelWidth - 32);

    setKnob (4, rightX + 12, 266);  setKnob (5, rightX + 96, 266);  setKnob (6, rightX + 180, 266);
    setButtonRow (10, 3, rightX + 14, 392, panelWidth - 28);
    setKnob (7, rightX + 178, 466);
    setKnob (9, rightX + 26, 574);  setKnob (10, rightX + 112, 574);  setKnob (8, rightX + 188, 574);

    buttons[16]->setBounds (width / 2 - 115, 748, 230, 42);
    buttons[13]->setBounds (rightX - 8, 750, 112, 38);
    setKnob (11, width - 132, 724);
}
void MaDriveAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (metal); auto r = getLocalBounds().reduced (15); g.setColour (dark); g.fillRoundedRectangle (r.toFloat(), 12); g.setColour (metal); g.fillRoundedRectangle (r.reduced (12).toFloat(), 8);
    g.setColour (juce::Colours::black); g.fillRect (getLocalBounds().withTrimmedLeft (getWidth() / 4).withTrimmedRight (getWidth() / 4).reduced (15));
    g.setColour (juce::Colours::white); g.setFont (juce::FontOptions (38).withStyle ("Bold")); g.drawText ("Ma Drive", 0, 20, getWidth(), 48, juce::Justification::centred); g.setFont (juce::FontOptions (13)); g.drawText ("WADIDAW / SATURATION / CLIPPER", 0, 67, getWidth(), 20, juce::Justification::centred);
    g.setColour (juce::Colours::white); g.drawText ("SOFT CLIP", 35, 115, 240, 28, juce::Justification::centred); g.drawText ("DRIVE", getWidth() - 275, 115, 240, 28, juce::Justification::centred);
    auto graph = juce::Rectangle<float> (335.0f, 150.0f, (float) getWidth() - 670.0f, 410.0f); g.setColour (juce::Colours::black); g.fillRect (graph); g.setColour (juce::Colours::darkgrey);
    for (int i = 1; i < 8; ++i) g.drawHorizontalLine ((int) (graph.getY() + i * graph.getHeight() / 8), graph.getX(), graph.getRight());
    juce::Path inputPath, outputPath; int write = p.historyWrite.load();
    for (int i = 0; i < 256; ++i) { int k = (write + i) & 255; float x = graph.getX() + graph.getWidth() * i / 255.0f; float inputY = graph.getCentreY() - p.inputHistory[(size_t) k] * 90; float outputY = graph.getCentreY() - p.outputHistory[(size_t) k] * 90; if (i == 0) { inputPath.startNewSubPath (x, inputY); outputPath.startNewSubPath (x, outputY); } else { inputPath.lineTo (x, inputY); outputPath.lineTo (x, outputY); } }
    const juce::PathStrokeType inputStroke (1.5f), outputStroke (2.0f); const float thresholdDashes[] { 5.0f, 4.0f };
    g.setColour (juce::Colours::whitesmoke); g.strokePath (inputPath, inputStroke); g.setColour (red); g.strokePath (outputPath, outputStroke);
    float thresholdY = graph.getCentreY() + p.thresholdForDisplay.load() * 4; g.setColour (amber); g.drawDashedLine ({ graph.getX(), thresholdY, graph.getRight(), thresholdY }, thresholdDashes, 2, 2.0f);
    g.setColour (juce::Colours::white); g.drawText ("WAVEFORM     INPUT     OUTPUT     THRESHOLD", (int) graph.getX(), 125, (int) graph.getWidth(), 20, juce::Justification::centred); g.setColour (amber); g.drawText ("IN  " + juce::String (p.inputPeak.load(), 1) + " dB       GAIN REDUCTION " + juce::String (p.reduction.load(), 1) + " dB       OUT  " + juce::String (p.outputPeak.load(), 1) + " dB", (int) graph.getX(), 575, (int) graph.getWidth(), 20, juce::Justification::centred);
}
