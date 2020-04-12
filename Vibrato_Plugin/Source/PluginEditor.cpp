/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Vibrato_pluginAudioProcessorEditor::Vibrato_pluginAudioProcessorEditor (Vibrato_pluginAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processor (p), valueTreeState(vts)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    addAndMakeVisible(modWidthSlider);
    addAndMakeVisible(modWidthLabel);
    modWidthLabel.setText("Modulation Width", dontSendNotification);
    modWidthLabel.attachToComponent(&modWidthSlider, true);
    modWidthSlider.setTextValueSuffix(" ms");
    modWidthAttachment.reset(new SliderAttachment(valueTreeState, "modWidth", modWidthSlider));
    modWidthSlider.setNumDecimalPlacesToDisplay(3);
    modWidthSlider.onValueChange = [this] {modWidthSliderChanged(); };

    addAndMakeVisible(freqSlider);
    addAndMakeVisible(freqLabel);
    freqLabel.setText("Frequency", dontSendNotification);
    freqLabel.attachToComponent(&freqSlider, true);
    freqSlider.setTextValueSuffix(" Hz");
    freqAttachment.reset(new SliderAttachment(valueTreeState, "modFreq", freqSlider));
    freqSlider.setNumDecimalPlacesToDisplay(3);
    freqSlider.onValueChange = [this] {freqSliderChanged(); };

    addAndMakeVisible(bypassButton);
    bypassButton.setButtonText("Bypass");
    bypassAttachment.reset(new ButtonAttachment(valueTreeState, "bypass", bypassButton));
    bypassButton.onClick = [this] {bypassButtonClicked(); };
    bypassButton.setEnabled(true);
}

Vibrato_pluginAudioProcessorEditor::~Vibrato_pluginAudioProcessorEditor()
{
}

//==============================================================================
void Vibrato_pluginAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.setColour (Colours::white);
}

void Vibrato_pluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto sliderLeft = 120;
    modWidthSlider.setBounds(sliderLeft, 20, getWidth() - sliderLeft - 10, 20);
    freqSlider.setBounds(sliderLeft, 50, getWidth() - sliderLeft - 10, 20);
    bypassButton.setBounds(10, 80, 100, 50);
}

/*! Function that is called when the bypass button is clicked */
void Vibrato_pluginAudioProcessorEditor::bypassButtonClicked()
{
    processor.toggleBypass();
}

/*! Function that is called when the mod width slider is adjusted */
void Vibrato_pluginAudioProcessorEditor::modWidthSliderChanged()
{
    processor.setWidth();
}

/*! Function that is called when the freq slider is adjusted */
void Vibrato_pluginAudioProcessorEditor::freqSliderChanged()
{
    processor.setFreq();
}
