/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DualBandDistortionAudioProcessor::DualBandDistortionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), treeState(*this, nullptr, juce::Identifier("PARAMETERS"),
                           { std::make_unique<juce::AudioParameterFloat>("cutoff", "Cutoff", 20.0f, 20000.0f, 200.0f),
                           std::make_unique<juce::AudioParameterChoice>("lowMode", "Low Mode", juce::StringArray("off", "half", "full"), 0),
                           std::make_unique<juce::AudioParameterChoice>("highMode", "High Mode", juce::StringArray("off", "half", "full"), 0) })
#endif
{
    treeState.addParameterListener("cutoff", this);
    treeState.addParameterListener("lowMode", this);
    treeState.addParameterListener("highMode", this);
}

DualBandDistortionAudioProcessor::~DualBandDistortionAudioProcessor()
{
}

//==============================================================================
const juce::String DualBandDistortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DualBandDistortionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DualBandDistortionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DualBandDistortionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DualBandDistortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DualBandDistortionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DualBandDistortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DualBandDistortionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DualBandDistortionAudioProcessor::getProgramName (int index)
{
    return {};
}

void DualBandDistortionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DualBandDistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    lwrFilter.reset();
    lwrFilter.prepare(spec);
}

void DualBandDistortionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DualBandDistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void DualBandDistortionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            float in = channelData[i];
            float low = 0.0f;
            float high = 0.0f;

            lwrFilter.processSample(channel, in, low, high);

            if (mLowDistChoice == 1)
            {
                if (low < 0)
                    low = 0.0f;
            }
            else if (mLowDistChoice == 2)
            {
                if (low < 0)
                    low = fabs(low);
            }

            if (mHighDistChoice == 1)
            {
                if (high < 0)
                    high = 0.0f;
            }
            else if (mHighDistChoice == 2)
            {
                if (high < 0)
                    high = fabs(high);
            }

            channelData[i] = low + high;
        }
    }
}

//==============================================================================
bool DualBandDistortionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DualBandDistortionAudioProcessor::createEditor()
{
    return new DualBandDistortionAudioProcessorEditor (*this, treeState);
}

//==============================================================================
void DualBandDistortionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DualBandDistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DualBandDistortionAudioProcessor();
}

// Function called when parameter is changed
void DualBandDistortionAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "cutoff")
        lwrFilter.setCutoffFrequency(newValue);

    else if (parameterID == "lowMode")
        mLowDistChoice = newValue;

    else if (parameterID == "highMode")
        mHighDistChoice = newValue;
    

}