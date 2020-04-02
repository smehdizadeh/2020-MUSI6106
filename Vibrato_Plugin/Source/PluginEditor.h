/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Vibrato_pluginAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    Vibrato_pluginAudioProcessorEditor (Vibrato_pluginAudioProcessor&);
    ~Vibrato_pluginAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    void modWidthSliderChanged();
    void freqSliderChanged();
    void bypassButtonClicked();

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Vibrato_pluginAudioProcessor& processor;

    // GUI members
    Slider modWidthSlider;
    Label modWidthLabel;

    Slider freqSlider;
    Label freqLabel;

    ToggleButton bypassButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Vibrato_pluginAudioProcessorEditor)
};
