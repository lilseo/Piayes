/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class PlugInTestingAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    PlugInTestingAudioProcessorEditor (PlugInTestingAudioProcessor&);
    ~PlugInTestingAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PlugInTestingAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlugInTestingAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
