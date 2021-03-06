/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
Delay_not_dumbAudioProcessor::Delay_not_dumbAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
//					   raw_volume(-5.0f),
					   state_tree(*this, nullptr)
#endif
{
	NormalisableRange<float> gain_range(-48.0f, 0.0f);
	state_tree.createAndAddParameter(GAIN_ID, GAIN_NAME, GAIN_NAME, gain_range, 0.5f, nullptr, nullptr);
	state_tree.state = ValueTree("saved_params");
}

Delay_not_dumbAudioProcessor::~Delay_not_dumbAudioProcessor()
{
}

//==============================================================================
const String Delay_not_dumbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Delay_not_dumbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Delay_not_dumbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Delay_not_dumbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Delay_not_dumbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Delay_not_dumbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Delay_not_dumbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Delay_not_dumbAudioProcessor::setCurrentProgram (int index)
{
}

const String Delay_not_dumbAudioProcessor::getProgramName (int index)
{
    return {};
}

void Delay_not_dumbAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void Delay_not_dumbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
	previous_gain = pow(10, *state_tree.getRawParameterValue(GAIN_ID) / 20);
}

void Delay_not_dumbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Delay_not_dumbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void Delay_not_dumbAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
	float current_gain = pow(10, *state_tree.getRawParameterValue(GAIN_ID) / 20);
	
    // in case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

	if (current_gain == previous_gain){
		buffer.applyGain(current_gain);
	}
	else {
		buffer.applyGainRamp(0, buffer.getNumSamples(), previous_gain, current_gain);
		previous_gain = current_gain;
	}
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...dfsdf
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
}

//==============================================================================
bool Delay_not_dumbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Delay_not_dumbAudioProcessor::createEditor()
{
    return new Delay_not_dumbAudioProcessorEditor (*this);
}

//==============================================================================
void Delay_not_dumbAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
	ScopedPointer <XmlElement> xml(state_tree.state.createXml());
	copyXmlToBinary(*xml, destData);
}

void Delay_not_dumbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
	ScopedPointer <XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

	if (xml != nullptr) {
		if (xml->hasTagName(state_tree.state.getType())){
			state_tree.state = ValueTree::fromXml(*xml);
		}
	}
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Delay_not_dumbAudioProcessor();
}
