/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <math.h>

//==============================================================================
Vibrato_pluginAudioProcessor::Vibrato_pluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    // initialization for member variables.
    m_fMaxModWidthInS = 0.5f;
    m_iNumChannels = 0;

    // empty initialization for changeable member variables.
    m_fSampleRate = 0.f;
    m_iNumberOfFrames = 0;
    m_bBypass = false;
    m_fModWidthInS = 0.f;

    m_ppfAudioData = 0;

    CVibrato::createInstance(m_pCVibrato);
}

Vibrato_pluginAudioProcessor::~Vibrato_pluginAudioProcessor()
{
    CVibrato::destroyInstance(m_pCVibrato);
}

//==============================================================================
const String Vibrato_pluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Vibrato_pluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Vibrato_pluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Vibrato_pluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Vibrato_pluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Vibrato_pluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Vibrato_pluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Vibrato_pluginAudioProcessor::setCurrentProgram (int index)
{
}

const String Vibrato_pluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void Vibrato_pluginAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void Vibrato_pluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    m_fSampleRate = sampleRate;
    m_iNumberOfFrames = samplesPerBlock;

    m_iNumChannels = getTotalNumInputChannels();

    m_ppfAudioData = new float*[m_iNumChannels];

    for (int i = 0; i < m_iNumChannels; i++)
    {
        m_ppfAudioData[i] = new float[m_iNumberOfFrames];
        for (int j = 0; j < m_iNumberOfFrames; j++)
            m_ppfAudioData[i][j] = 0.f;
    }

    m_pCVibrato->initInstance(m_fMaxModWidthInS, m_fSampleRate, m_iNumChannels);
}

void Vibrato_pluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    m_pCVibrato->resetInstance();

    for (int i = 0; i < m_iNumChannels; i++)
        delete m_ppfAudioData[i];

    delete[] m_ppfAudioData;
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Vibrato_pluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Vibrato_pluginAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    m_pCVibrato->process(buffer.getArrayOfWritePointers(), m_ppfAudioData, m_iNumberOfFrames);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        buffer.copyFrom(channel, 0, m_ppfAudioData[channel], m_iNumberOfFrames);
    }
    
}

//==============================================================================
bool Vibrato_pluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Vibrato_pluginAudioProcessor::createEditor()
{
    return new Vibrato_pluginAudioProcessorEditor (*this);
}

//==============================================================================
void Vibrato_pluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Vibrato_pluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

const double Vibrato_pluginAudioProcessor::getSampleRate()
{
    return m_fSampleRate;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Vibrato_pluginAudioProcessor();
}


Error_t Vibrato_pluginAudioProcessor::setWidth(float fModWidthInS)
{
    m_fModWidthInS = fModWidthInS;

    if (m_bBypass)
        return kNoError;

    return m_pCVibrato->setParam(CVibrato::kParamModWidthInS, fModWidthInS);
}

Error_t Vibrato_pluginAudioProcessor::setFreq(float fModFreqInHz)
{
    return m_pCVibrato->setParam(CVibrato::kParamModFreqInHz, fModFreqInHz);
}

Error_t Vibrato_pluginAudioProcessor::toggleBypass()
{
    m_bBypass = !m_bBypass;

    if (m_bBypass)
        return m_pCVibrato->setParam(CVibrato::kParamModWidthInS, 0.f);
    else
        return m_pCVibrato->setParam(CVibrato::kParamModWidthInS, m_fModWidthInS);
}
