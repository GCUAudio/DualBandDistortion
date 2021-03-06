/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DualBandDistortionAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DualBandDistortionAudioProcessorEditor (DualBandDistortionAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~DualBandDistortionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DualBandDistortionAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& treeState;

    juce::Slider cutoffSlider;
    juce::ComboBox lowDistCombo;
    juce::ComboBox highDistCombo;

    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> cutoffValue;
    std::unique_ptr <juce::AudioProcessorValueTreeState::ComboBoxAttachment> lowDistChoice;
    std::unique_ptr <juce::AudioProcessorValueTreeState::ComboBoxAttachment> highDistChoice;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DualBandDistortionAudioProcessorEditor)
};
