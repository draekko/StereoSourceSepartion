/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
StereoSourceSeparationAudioProcessor::StereoSourceSeparationAudioProcessor()
    :NUM_CHANNELS(2),
    BLOCK_SIZE(4096),
    HOP_SIZE(1024)
{
}

StereoSourceSeparationAudioProcessor::~StereoSourceSeparationAudioProcessor()
{
}

//==============================================================================
const String StereoSourceSeparationAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int StereoSourceSeparationAudioProcessor::getNumParameters()
{
    return 0;
}

float StereoSourceSeparationAudioProcessor::getParameter (int index)
{
    return 0.0f;
}

void StereoSourceSeparationAudioProcessor::setParameter (int index, float newValue)
{
}

const String StereoSourceSeparationAudioProcessor::getParameterName (int index)
{
    return String::empty;
}

const String StereoSourceSeparationAudioProcessor::getParameterText (int index)
{
    return String::empty;
}

const String StereoSourceSeparationAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String StereoSourceSeparationAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool StereoSourceSeparationAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool StereoSourceSeparationAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool StereoSourceSeparationAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool StereoSourceSeparationAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool StereoSourceSeparationAudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double StereoSourceSeparationAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int StereoSourceSeparationAudioProcessor::getNumPrograms()
{
    return 0;
}

int StereoSourceSeparationAudioProcessor::getCurrentProgram()
{
    return 0;
}

void StereoSourceSeparationAudioProcessor::setCurrentProgram (int index)
{
}

const String StereoSourceSeparationAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void StereoSourceSeparationAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void StereoSourceSeparationAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    separator_ = new ADRess(BLOCK_SIZE, 100);
}

void StereoSourceSeparationAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    delete separator_;
}

void StereoSourceSeparationAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int numSamples = buffer.getNumSamples();
    int inWritePos, samplesSinceFft, outReadPos, outWritePos;
    
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    for (int channel = 0; channel < getNumInputChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }

    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
}

//==============================================================================
bool StereoSourceSeparationAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* StereoSourceSeparationAudioProcessor::createEditor()
{
    return new StereoSourceSeparationAudioProcessorEditor (this);
}

//==============================================================================
void StereoSourceSeparationAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void StereoSourceSeparationAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StereoSourceSeparationAudioProcessor();
}
