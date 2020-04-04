/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Vibrato.h"

//==============================================================================
/**
*/
class Vibrato_pluginAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    Vibrato_pluginAudioProcessor();
    ~Vibrato_pluginAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    const double getSampleRate();

    Error_t setWidth(float fModWidthInS);
    Error_t setFreq(float fModFreqInHz);
    Error_t toggleBypass();

private:
    //==============================================================================
    CVibrato* m_pCVibrato;

    float m_fMaxModWidthInS;
    float m_fSampleRate;
    int m_iNumChannels;
    int m_iNumberOfFrames;

    bool m_bBypass;

    float** m_ppfAudioData;

    float m_fModWidthInS;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Vibrato_pluginAudioProcessor)
};
