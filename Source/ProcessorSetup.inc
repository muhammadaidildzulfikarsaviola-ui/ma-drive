namespace
{
juce::AudioParameterFloat* F(float a,float b,float d,const char* id,const char* name)
{
    return new juce::AudioParameterFloat(id,name,juce::NormalisableRange<float>(a,b,0.01f),d);
}
}

juce::AudioProcessorValueTreeState::ParameterLayout MaDriveAudioProcessor::makeLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    p.emplace_back(F(-24.0f,24.0f,0.0f,"input","Input"));
    p.emplace_back(F(-24.0f,0.0f,-8.0f,"threshold","Threshold"));
    p.emplace_back(F(-12.0f,12.0f,0.0f,"knee","Knee"));
    p.emplace_back(F(-24.0f,24.0f,0.0f,"clipOutput","Clip Output"));
    p.push_back(std::make_unique<juce::AudioParameterChoice>("clipStyle","Clip Style",juce::StringArray{"Soft","Medium","Hard"},0));
    p.push_back(std::make_unique<juce::AudioParameterChoice>("oversampling","Oversampling",juce::StringArray{"1x","2x","4x","8x"},1));
    p.push_back(std::make_unique<juce::AudioParameterChoice>("processMode","Process Mode",juce::StringArray{"Stereo","M/S","Multi Band"},0));
    p.emplace_back(F(0.0f,24.0f,4.0f,"drive","Drive"));
    p.emplace_back(F(0.0f,1.0f,0.5f,"tone","Tone"));
    p.emplace_back(F(-24.0f,24.0f,0.0f,"driveOutput","Drive Output"));
    p.emplace_back(F(0.0f,100.0f,100.0f,"mix","Mix"));
    p.emplace_back(F(0.0f,100.0f,35.0f,"character","Character"));
    p.emplace_back(F(0.0f,500.0f,40.0f,"hpf","HPF"));
    p.emplace_back(F(0.0f,20000.0f,0.0f,"lpf","LPF"));
    p.push_back(std::make_unique<juce::AudioParameterChoice>("driveMode","Drive Mode",juce::StringArray{"Growl","Odd","Tape"},0));
    p.emplace_back(F(-12.0f,12.0f,0.0f,"trim","Trim"));
    p.push_back(std::make_unique<juce::AudioParameterBool>("bypass","Bypass",false));
    return juce::AudioProcessorValueTreeState::ParameterLayout(p.begin(),p.end());
}

MaDriveAudioProcessor::MaDriveAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true)
                                     .withOutput("Output",juce::AudioChannelSet::stereo(),true)),
      parameters(*this,nullptr,"PARAMETERS",makeLayout())
{
    stateA=parameters.copyState();
    stateB=stateA.createCopy();
}

bool MaDriveAudioProcessor::hasEditor() const{return true;}

bool MaDriveAudioProcessor::isBusesLayoutSupported(const BusesLayout& l) const
{
    auto in=l.getMainInputChannelSet();
    auto out=l.getMainOutputChannelSet();
    return in==out && (in==juce::AudioChannelSet::mono() || in==juce::AudioChannelSet::stereo());
}

void MaDriveAudioProcessor::prepareToPlay(double sr,int block)
{
    juce::ignoreUnused(sr);
    for(int i=0;i<4;++i)
    {
        oversampling[i]=std::make_unique<juce::dsp::Oversampling<float>>(2,(size_t)i,juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR,true);
        oversampling[i]->initProcessing((size_t)block);
    }
    hpfL.reset(); hpfR.reset(); lpfL.reset(); lpfR.reset();
    bandLowL.reset(); bandLowR.reset(); bandHighL.reset(); bandHighR.reset();
    dryBuffer.setSize(2,block,false,false,true);
}

float MaDriveAudioProcessor::shape(float x,int cs,int dm,float ch) const
{
    float y;
    if(dm==0) y=std::tanh(x*(1.0f+ch*1.5f));
    else if(dm==1) y=x-x*x*x*(0.18f+0.12f*ch);
    else y=std::atan(x*(1.0f+ch))*0.78f;
    if(cs==0) return std::tanh(y);
    if(cs==1) return y/(1.0f+std::abs(y));
    return juce::jlimit(-1.0f,1.0f,y);
}
