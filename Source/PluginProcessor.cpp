#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
StereoReverbAudioProcessor::StereoReverbAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       ),
#endif
    apvts(*this, nullptr, "Parameters", createParameters())
{
}

StereoReverbAudioProcessor::~StereoReverbAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout StereoReverbAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "roomSize", "Room Size", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        "decayTime", "Decay Time", juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f), 2.0f));

    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String StereoReverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool StereoReverbAudioProcessor::acceptsMidi() const
{
    return false;
}

bool StereoReverbAudioProcessor::producesMidi() const
{
    return false;
}

bool StereoReverbAudioProcessor::isMidiEffect() const
{
    return false;
}

double StereoReverbAudioProcessor::getTailLengthSeconds() const
{
    return 10.0;
}

int StereoReverbAudioProcessor::getNumPrograms()
{
    return 1;
}

int StereoReverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void StereoReverbAudioProcessor::setCurrentProgram (int)
{
}

const juce::String StereoReverbAudioProcessor::getProgramName (int)
{
    return {};
}

void StereoReverbAudioProcessor::changeProgramName (int, const juce::String&)
{
}

//==============================================================================
void StereoReverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    reverbL.reset();
    reverbR.reset();

    reverbL.prepare(spec);
    reverbR.prepare(spec);
}

void StereoReverbAudioProcessor::releaseResources()
{
    reverbL.reset();
    reverbR.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool StereoReverbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Only stereo in/out supported
    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo()
        || layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}
#endif

void StereoReverbAudioProcessor::updateReverbParameters()
{
    juce::Reverb::Parameters params;
    params.roomSize = apvts.getRawParameterValue("roomSize")->load();
    // Map decayTime (0.1-10s) to damping and wetLevel for a more natural decay
    float decay = apvts.getRawParameterValue("decayTime")->load();
    params.damping = juce::jlimit(0.1f, 0.99f, 1.0f - (decay / 10.0f));
    params.wetLevel = 0.33f;
    params.dryLevel = 0.67f;
    params.width = 1.0f;
    params.freezeMode = 0.0f;

    reverbL.setParameters(params);
    reverbR.setParameters(params);
}

void StereoReverbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    updateReverbParameters();

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto* left = buffer.getWritePointer(0);
    auto* right = buffer.getWritePointer(1);

    juce::dsp::AudioBlock<float> block(buffer);

    // Process each channel separately for true stereo reverb
    juce::dsp::AudioBlock<float> leftBlock = block.getSingleChannelBlock(0);
    juce::dsp::AudioBlock<float> rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    reverbL.process(leftContext);
    reverbR.process(rightContext);
}

//==============================================================================
bool StereoReverbAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* StereoReverbAudioProcessor::createEditor()
{
    return new StereoReverbAudioProcessorEditor (*this);
}

//==============================================================================
void StereoReverbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void StereoReverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StereoReverbAudioProcessor();
}
```

```cpp