/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define FFT_SIZE 1024


//==============================================================================
StereoSourceSeparationAudioProcessor::StereoSourceSeparationAudioProcessor()
    :NUM_CHANNELS(2),
    BLOCK_SIZE(FFT_SIZE),
    HOP_SIZE(FFT_SIZE/2),
    inputBuffer_(2,FFT_SIZE),
    outputBuffer_(2,FFT_SIZE*2)
{
    inputBufferLength_ = FFT_SIZE;
    outputBufferLength_ = FFT_SIZE*2;
    inputBufferWritePosition_ = outputBufferWritePosition_ = outputBufferReadPosition_ = 0;
    samplesSinceLastFFT_ = 0;
    
    float* outputBufferData[NUM_CHANNELS];
    outputBufferData[0] = outputBuffer_.getWritePointer(0);
    outputBufferData[1] = outputBuffer_.getWritePointer(1);
    for (int i = 0; i<outputBufferLength_; i++) {
        outputBufferData[0][i] = 0;
        outputBufferData[1][i] = 0;
    }
    
    
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
    processBuffer_ = new float*[NUM_CHANNELS];
    for (int c = 0; c<NUM_CHANNELS; c++) {
        processBuffer_[c] = new float[BLOCK_SIZE];
    }
}

void StereoSourceSeparationAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    delete separator_;
    for (int c = 0; c<NUM_CHANNELS; c++) {
        delete [] processBuffer_[c];
    }
    delete [] processBuffer_;
}

void StereoSourceSeparationAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int numSamples = buffer.getNumSamples();
    
    float* stereoData[NUM_CHANNELS];
    stereoData[0] = buffer.getWritePointer(0);
    stereoData[1] = buffer.getWritePointer(1);
    float* inputBufferData[NUM_CHANNELS];
    inputBufferData[0] = inputBuffer_.getWritePointer(0);
    inputBufferData[1] = inputBuffer_.getWritePointer(1);
    float* outputBufferData[NUM_CHANNELS];
    outputBufferData[0] = outputBuffer_.getWritePointer(0);
    outputBufferData[1] = outputBuffer_.getWritePointer(1);
    
    for (int i = 0; i<numSamples; i++) {
        
        // store input sample data, output sample from output buffer, then clear output buffer sample
        inputBufferData[0][inputBufferWritePosition_] = stereoData[0][i];
        inputBufferData[1][inputBufferWritePosition_] = stereoData[1][i];
        if (++inputBufferWritePosition_ >= inputBufferLength_)
            inputBufferWritePosition_ = 0;
        
        stereoData[0][i] = outputBufferData[0][outputBufferReadPosition_];
        stereoData[1][i] = outputBufferData[1][outputBufferReadPosition_];
        
        outputBufferData[0][outputBufferReadPosition_] = 0.0;
        outputBufferData[1][outputBufferReadPosition_] = 0.0;
        if (++outputBufferReadPosition_ >= outputBufferLength_)
            outputBufferReadPosition_ = 0;
        
        // samples since last fft exceeds hopsize, do fft
        if (++samplesSinceLastFFT_ >= HOP_SIZE) {
            samplesSinceLastFFT_ = 0;
            
            // find start positino in input buffer, then load the process buffer leftData_ and rightData_
            int inputBufferIndex = (inputBufferWritePosition_ - BLOCK_SIZE + inputBufferLength_) % inputBufferLength_;
            for (int procBufferIndex = 0; procBufferIndex < BLOCK_SIZE; procBufferIndex++) {
                processBuffer_[0][procBufferIndex] = inputBufferData[0][inputBufferIndex];
                processBuffer_[1][procBufferIndex] = inputBufferData[1][inputBufferIndex];
                if ( ++inputBufferIndex >= inputBufferLength_)
                    inputBufferIndex = 0;
            }
            
            separator_->process(processBuffer_[0], processBuffer_[1]);
            
            int outputBufferIndex = outputBufferWritePosition_;
            for (int procBufferIndex = 0; procBufferIndex < BLOCK_SIZE; procBufferIndex++) {
                outputBufferData[0][outputBufferIndex] += processBuffer_[0][procBufferIndex];
                outputBufferData[1][outputBufferIndex] += processBuffer_[1][procBufferIndex];
                
                if (++outputBufferIndex >= outputBufferLength_)
                    outputBufferIndex = 0;
            }
            
            // advance write position by hop size
            outputBufferWritePosition_ = (outputBufferWritePosition_ + HOP_SIZE) % outputBufferLength_;
        }
        
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
