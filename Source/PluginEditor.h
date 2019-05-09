/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class JuceDelayAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    JuceDelayAudioProcessorEditor (JuceDelayAudioProcessor&);
    ~JuceDelayAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    
    enum {
        editorWidth = 500,
        editorMargin = 10,
        editorPadding = 10,
        
        sliderTextEntryBoxWidth = 100,
        sliderTextEntryBoxHeight = 25,
        sliderHeight = 25,
        buttonHeight = 25,
        comboBoxHeight = 25,
        labelWidth = 100,
    };
    
    
    
    OwnedArray<Slider> sliders;
    OwnedArray<ToggleButton> toggles;
    OwnedArray<ComboBox> comboBoxes;
    
    OwnedArray<Label> labels;
    Array<Component*> components;
    
    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    
    OwnedArray<SliderAttachment> sliderAttachments;
    OwnedArray<ButtonAttachment> buttonAttachments;
    OwnedArray<ComboBoxAttachment> comboBoxAttachments;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    JuceDelayAudioProcessor& processor;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JuceDelayAudioProcessorEditor)
};
