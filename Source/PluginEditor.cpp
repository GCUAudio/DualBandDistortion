/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DualBandDistortionAudioProcessorEditor::DualBandDistortionAudioProcessorEditor (DualBandDistortionAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), treeState(vts)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 200);

    // Cutoff Frequency
    cutoffValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(treeState, "cutoff", cutoffSlider);
    cutoffSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    cutoffSlider.setRange(20.0f, 20000.0f, 0.0f);
    cutoffSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxAbove, true, 80, 20);
    addAndMakeVisible(&cutoffSlider);

    // Low dist selection
    lowDistCombo.addItem("No processing", 1);
    lowDistCombo.addItem("Half Wave", 2);
    lowDistCombo.addItem("Full Wave", 3);
    lowDistChoice = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(treeState, "lowMode", lowDistCombo);
    addAndMakeVisible(&lowDistCombo);
    
    // High dist selection
    highDistCombo.addItem("No processing", 1);
    highDistCombo.addItem("Half Wave", 2);
    highDistCombo.addItem("Full Wave", 3);
    highDistChoice = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(treeState, "highMode", highDistCombo);
    addAndMakeVisible(&highDistCombo);

}

DualBandDistortionAudioProcessorEditor::~DualBandDistortionAudioProcessorEditor()
{
}

//==============================================================================
void DualBandDistortionAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void DualBandDistortionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    cutoffSlider.setBounds(10, 30, 380, 50);
    lowDistCombo.setBounds(10, 85, 130, 30);
    highDistCombo.setBounds(260, 85, 130, 30);
}
