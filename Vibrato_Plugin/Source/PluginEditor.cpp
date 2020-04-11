/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Vibrato_pluginAudioProcessorEditor::Vibrato_pluginAudioProcessorEditor (Vibrato_pluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    addAndMakeVisible(modWidthSlider);
    addAndMakeVisible(modWidthLabel);
    modWidthLabel.setText("Modulation Width", dontSendNotification);
    modWidthLabel.attachToComponent(&modWidthSlider, true);
    modWidthSlider.setTextValueSuffix(" ms");
    modWidthSlider.setRange(p.getParamRange(CVibrato::kParamModWidthInS, 0)*1000.f, p.getParamRange(CVibrato::kParamModWidthInS, 1)*1000.f);
    modWidthSlider.setNumDecimalPlacesToDisplay(3);
    modWidthSlider.onValueChange = [this] {modWidthSliderChanged(); };

    addAndMakeVisible(freqSlider);
    addAndMakeVisible(freqLabel);
    freqLabel.setText("Frequency", dontSendNotification);
    freqLabel.attachToComponent(&freqSlider, true);
    freqSlider.setTextValueSuffix(" Hz");
    freqSlider.setRange(p.getParamRange(CVibrato::kParamModFreqInHz, 0), p.getParamRange(CVibrato::kParamModFreqInHz, 1));
    freqSlider.setNumDecimalPlacesToDisplay(3);
    freqSlider.onValueChange = [this] {freqSliderChanged(); };

    addAndMakeVisible(bypassButton);
    bypassButton.setButtonText("Bypass");
    if (p.isBypassed())
    {
        bypassButton.setState(Button::ButtonState::buttonDown);
    }
    else
        bypassButton.setState(Button::ButtonState::buttonNormal);
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
    processor.setWidth(modWidthSlider.getValue() / 1000.f);
}

/*! Function that is called when the freq slider is adjusted */
void Vibrato_pluginAudioProcessorEditor::freqSliderChanged()
{
    processor.setFreq(freqSlider.getValue());
}
