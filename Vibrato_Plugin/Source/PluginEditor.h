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
    Vibrato_pluginAudioProcessorEditor (Vibrato_pluginAudioProcessor&, AudioProcessorValueTreeState&);
    ~Vibrato_pluginAudioProcessorEditor();

    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

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
    std::unique_ptr<SliderAttachment> modWidthAttachment;

    Slider freqSlider;
    Label freqLabel;
    std::unique_ptr<SliderAttachment> freqAttachment;

    ToggleButton bypassButton;
    std::unique_ptr<ButtonAttachment> bypassAttachment;

    AudioProcessorValueTreeState& valueTreeState;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Vibrato_pluginAudioProcessorEditor)
};
