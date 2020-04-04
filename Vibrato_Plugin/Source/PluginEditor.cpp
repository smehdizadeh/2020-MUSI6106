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
    modWidthSlider.setTextValueSuffix(" sec");
    modWidthSlider.setRange(0, 0.5f); //from limits in CVibrato test suite.
    modWidthSlider.setNumDecimalPlacesToDisplay(3);
    modWidthSlider.onValueChange = [this] {modWidthSliderChanged(); };

    addAndMakeVisible(freqSlider);
    addAndMakeVisible(freqLabel);
    freqLabel.setText("Frequency", dontSendNotification);
    freqLabel.attachToComponent(&freqSlider, true);
    freqSlider.setTextValueSuffix(" Hz");
    freqSlider.setRange(0, processor.getSampleRate() * .5F); //from limits in CVibrato initInstance()
    freqSlider.setNumDecimalPlacesToDisplay(3);
    freqSlider.onValueChange = [this] {freqSliderChanged(); };

    addAndMakeVisible(bypassButton);
    bypassButton.setButtonText("Bypass");
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
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
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
    processor.setParam(CVibrato::VibratoParam_t::kParamModWidthInS, modWidthSlider.getValue());
}

/*! Function that is called when the freq slider is adjusted */
void Vibrato_pluginAudioProcessorEditor::freqSliderChanged()
{
    processor.setParam(CVibrato::VibratoParam_t::kParamModFreqInHz, freqSlider.getValue());
}