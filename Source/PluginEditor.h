/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include <math.h>

//==============================================================================
/**
*/
class StereoSourceSeparationAudioProcessorEditor  : public AudioProcessorEditor,
                                                    public ButtonListener,
                                                    public SliderListener,
                                                    public MouseListener
{
public:
    StereoSourceSeparationAudioProcessorEditor (StereoSourceSeparationAudioProcessor* ownerFilter);
    ~StereoSourceSeparationAudioProcessorEditor();

    //==============================================================================
    // This is just a standard Juce paint method...
    void paint (Graphics&) override;
    void resized() override;
    void mouseDrag(const juce::MouseEvent &e);
    void sliderValueChanged (Slider*) override;
    void buttonClicked (Button* buttonThatWasClicked);

private:
    int width_;
    int height_;
    float dirAngle;
    float widAngle;
    float radius;
    Colour paintColour;
    
    Path internalPath;
    Path arrowPath;
    Line<float> arrowLine;
    ScopedPointer<Label> label;
    ScopedPointer<Slider> widthSlider;
    ScopedPointer<ToggleButton> soloToggle;
    ScopedPointer<ToggleButton> muteToggle;
    ScopedPointer<ToggleButton> bypassToggle;
    ScopedPointer<ToggleButton> allpassToggle;
    ScopedPointer<ToggleButton> lowpassToggle;
    ScopedPointer<ToggleButton> highpassToggle;
    ScopedPointer<Slider> cutofffreqSlider;
    ScopedPointer<TextButton> resetButton;
    ScopedPointer<Label> dirLabel;
    ScopedPointer<Label> widLabel;
    ScopedPointer<Label> dirVal;
    ScopedPointer<Label> widVal;
    ScopedPointer<Label> sideVal;
    ScopedPointer<Label> freqLabel;
    ScopedPointer<Label> freqVal;
    ScopedPointer<Label> lowboundLabel;
    ScopedPointer<Label> highboundLabel;
    ScopedPointer<Label> modeLabel;
    ScopedPointer<Label> filterLabel;

    StereoSourceSeparationAudioProcessor* getProcessor() const
    {
        return static_cast <StereoSourceSeparationAudioProcessor*> (getAudioProcessor());
    }
};


#endif  // PLUGINEDITOR_H_INCLUDED
