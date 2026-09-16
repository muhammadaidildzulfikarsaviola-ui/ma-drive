#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
void lifecycleLog (const juce::String& message)
{
    auto logFile = juce::File::getSpecialLocation (juce::File::tempDirectory)
                       .getChildFile ("MaDrive-lifecycle.log");
    logFile.appendText (juce::Time::getCurrentTime().toString (true, true)
                        + " " + message + "\n");
    DBG (message);
}
}

namespace { auto range (float a, float b, float d, const char* id, const char* name) { return std::make_unique<juce::AudioParameterFloat>(id, name, juce::NormalisableRange<float>(a,b,0.01f), d); } }
juce::AudioProcessorValueTreeState::ParameterLayout MaDriveAudioProcessor::makeLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    p.push_back(range(-24,24,0,"input","Input")); p.push_back(range(-24,0,-8,"threshold","Threshold")); p.push_back(range(0,1,0.45f,"knee","Knee")); p.push_back(range(-24,24,0,"clipOutput","Clip Output"));
    p.push_back(std::make_unique<juce::AudioParameterChoice>("clipStyle","Clip Style", juce::StringArray{"Soft","Medium","Hard"},0));
    p.push_back(std::make_unique<juce::AudioParameterChoice>("oversampling","Oversampling",juce::StringArray{"1x","2x","4x","8x"},1));
    p.push_back(std::make_unique<juce::AudioParameterChoice>("processMode","Process Mode",juce::StringArray{"Stereo","M/S","Multi Band"},0));
    p.push_back(range(0,24,4,"drive","Drive")); p.push_back(range(0,1,0.5f,"tone","Tone")); p.push_back(range(-24,24,0,"driveOutput","Drive Output")); p.push_back(range(0,1,1,"mix","Mix")); p.push_back(range(0,1,0.35f,"character","Character"));
    p.push_back(range(20,500,40,"hpf","HPF")); p.push_back(range(1000,20000,18000,"lpf","LPF"));
    p.push_back(std::make_unique<juce::AudioParameterChoice>("driveMode","Drive Mode",juce::StringArray{"Growl","Odd","Tape"},0));
    p.push_back(range(-12,12,0,"trim","Trim")); p.push_back(std::make_unique<juce::AudioParameterBool>("bypass","Bypass",false));
    return { p.begin(), p.end() };
}
MaDriveAudioProcessor::MaDriveAudioProcessor() : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)), parameters(*this,nullptr,"PARAMETERS",makeLayout()) { lifecycleLog ("MaDriveAudioProcessor constructor entered"); stateA=parameters.copyState(); stateB=stateA.createCopy(); lifecycleLog ("MaDriveAudioProcessor constructor completed"); }
bool MaDriveAudioProcessor::hasEditor() const { lifecycleLog ("hasEditor returning true"); return true; }
bool MaDriveAudioProcessor::isBusesLayoutSupported(const BusesLayout& l) const { return l.getMainInputChannelSet()==l.getMainOutputChannelSet() && (l.getMainInputChannelSet()==juce::AudioChannelSet::mono() || l.getMainInputChannelSet()==juce::AudioChannelSet::stereo()); }
void MaDriveAudioProcessor::prepareToPlay(double sr,int block) { for(int i=0;i<4;++i) { oversampling[i]=std::make_unique<juce::dsp::Oversampling<float>>(2, (size_t)i, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true); oversampling[i]->initProcessing((size_t)block); } hpfL.reset();hpfR.reset();lpfL.reset();lpfR.reset();bandLowL.reset();bandLowR.reset();bandHighL.reset();bandHighR.reset();dryBuffer.setSize(2,block,false,false,true); }
float MaDriveAudioProcessor::shape(float x,int cs,int dm,float ch) const { float y; if(dm==0) y=std::tanh(x*(1.0f+ch*1.5f)); else if(dm==1) y=x-(x*x*x)*(0.18f+0.12f*ch); else y=std::atan(x*(1.0f+ch))*0.78f; if(cs==0) return std::tanh(y); if(cs==1) return y/(1.0f+std::abs(y)); return juce::jlimit(-1.0f,1.0f,y); }
void MaDriveAudioProcessor::processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer&) {
    juce::ScopedNoDenormals nd; auto n=b.getNumSamples(), ch=b.getNumChannels();
    auto v=[this](const char* id){return parameters.getRawParameterValue(id)->load();};
    if (v("bypass") > 0.5f)
    {
        float peak = 0.0f;
        for (int c = 0; c < ch; ++c) for (int i = 0; i < n; ++i) peak = juce::jmax (peak, std::abs (b.getSample (c, i)));
        const auto level = juce::Decibels::gainToDecibels (juce::jmax (peak, 1.0e-6f));
        inputPeak.store (level); outputPeak.store (level); reduction.store (0.0f);
        monitorPeaks[0].store (level); monitorPeaks[1].store (level); monitorPeaks[2].store (level);
        return;
    }
    const float inG=juce::Decibels::decibelsToGain(v("input")), driveG=juce::Decibels::decibelsToGain(v("drive")), outG=juce::Decibels::decibelsToGain(v("clipOutput")+v("driveOutput")+v("trim")); const float threshold=juce::Decibels::decibelsToGain(v("threshold")); thresholdForDisplay.store(v("threshold")); dryBuffer.makeCopyOf(b, true);
    auto h=juce::dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(),v("hpf")); auto l=juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(),v("lpf")); hpfL.coefficients=hpfR.coefficients=h; lpfL.coefficients=lpfR.coefficients=l; auto bl=juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(),180.0);auto bh=juce::dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(),2600.0);bandLowL.coefficients=bandLowR.coefficients=bl;bandHighL.coefficients=bandHighR.coefficients=bh;
    float before=0, after=0, stagePeak=0, lowPeak=0, midPeak=0, highPeak=0; int pm=(int)v("processMode");
    if(pm==1 && ch==2) for(int i=0;i<n;++i){float L=b.getSample(0,i),R=b.getSample(1,i); b.setSample(0,i,(L+R)*0.7071067f);b.setSample(1,i,(L-R)*0.7071067f);}
    for(int c=0;c<ch;++c) for(int i=0;i<n;++i){ float x=b.getSample(c,i)*inG; b.setSample(c,i,x); before=juce::jmax(before,std::abs(x)); }
    auto block=juce::dsp::AudioBlock<float>(b); int os=juce::jlimit(0,3,(int)v("oversampling")); auto up=oversampling[(size_t)os]->processSamplesUp(block);
    for(size_t c=0;c<up.getNumChannels();++c) for(size_t i=0;i<up.getNumSamples();++i){float x=up.getSample(c,i); x=(c==0?hpfL.processSample(x):hpfR.processSample(x)); const float kneeThreshold=threshold*(1.0f+0.65f*v("knee")); auto nonlinear=[&](float z,float amount){return shape(z/juce::jmax(0.05f,kneeThreshold),(int)v("clipStyle"),(int)v("driveMode"),v("character"))*kneeThreshold*amount;}; if(pm==2){float low=(c==0?bandLowL.processSample(x):bandLowR.processSample(x));float high=(c==0?bandHighL.processSample(x):bandHighR.processSample(x));float mid=x-low-high;lowPeak=juce::jmax(lowPeak,std::abs(low));midPeak=juce::jmax(midPeak,std::abs(mid));highPeak=juce::jmax(highPeak,std::abs(high));x=nonlinear(low,0.75f)+nonlinear(mid,1.0f)+nonlinear(high,1.3f);}else x=nonlinear(x,1.0f); x*=driveG*(0.75f+0.5f*v("tone")); x=(c==0?lpfL.processSample(x):lpfR.processSample(x)); up.setSample(c,i,x);}
    oversampling[(size_t)os]->processSamplesDown(block);
    std::array<float, 2> channelPeaks { 0.0f, 0.0f };
    for (int c = 0; c < ch; ++c) for (int i = 0; i < n; ++i) { const auto sample = std::abs (b.getSample (c, i)); stagePeak = juce::jmax (stagePeak, sample); if (c < 2) channelPeaks[(size_t) c] = juce::jmax (channelPeaks[(size_t) c], sample); }
    for(int c=0;c<ch;++c) for(int i=0;i<n;++i){float wet=b.getSample(c,i); float dry=dryBuffer.getSample(c,i); float y=(wet*v("mix")+dry*(1-v("mix")))*outG; if(!std::isfinite(y)) y=0; b.setSample(c,i,y);after=juce::jmax(after,std::abs(y));}
    if(pm==1 && ch==2) for(int i=0;i<n;++i){float M=b.getSample(0,i),S=b.getSample(1,i);b.setSample(0,i,(M+S)*0.7071067f);b.setSample(1,i,(M-S)*0.7071067f);}
    const auto gr = juce::jmax (0.0f, juce::Decibels::gainToDecibels (juce::jmax (before, 1.0e-6f)) - juce::Decibels::gainToDecibels (juce::jmax (stagePeak, 1.0e-6f)));
    inputPeak.store(juce::Decibels::gainToDecibels(before));outputPeak.store(juce::Decibels::gainToDecibels(after));reduction.store(-gr);
    if (pm == 2) { monitorPeaks[0].store (juce::Decibels::gainToDecibels (juce::jmax (lowPeak, 1.0e-6f))); monitorPeaks[1].store (juce::Decibels::gainToDecibels (juce::jmax (midPeak, 1.0e-6f))); monitorPeaks[2].store (juce::Decibels::gainToDecibels (juce::jmax (highPeak, 1.0e-6f))); } else { monitorPeaks[0].store (juce::Decibels::gainToDecibels (juce::jmax (channelPeaks[0], 1.0e-6f))); monitorPeaks[1].store (juce::Decibels::gainToDecibels (juce::jmax (channelPeaks[1], 1.0e-6f))); monitorPeaks[2].store (-100.0f); }
    for(int i=0;i<n;i+=juce::jmax(1,n/8)){int w=historyWrite.fetch_add(1)&255;inputHistory[(size_t)w]=dryBuffer.getSample(0,i);outputHistory[(size_t)w]=b.getSample(0,i);grHistory[(size_t)w]=-gr;}
}
void MaDriveAudioProcessor::getStateInformation(juce::MemoryBlock& d){auto x=parameters.copyState();std::unique_ptr<juce::XmlElement> xml(x.createXml());copyXmlToBinary(*xml,d);} void MaDriveAudioProcessor::setStateInformation(const void* d,int s){if(auto x=getXmlFromBinary(d,s)) parameters.replaceState(juce::ValueTree::fromXml(*x));}
void MaDriveAudioProcessor::copyAB(bool b){if(b)stateB=parameters.copyState();else stateA=parameters.copyState();}void MaDriveAudioProcessor::recallAB(bool b){parameters.replaceState((b?stateB:stateA).createCopy());}
juce::AudioProcessorEditor* MaDriveAudioProcessor::createEditor(){lifecycleLog ("createEditor entered"); auto* editor = new MaDriveAudioProcessorEditor(*this); lifecycleLog ("createEditor completed"); return editor;} 

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    lifecycleLog ("createPluginFilter entered");
    return new MaDriveAudioProcessor();
}
