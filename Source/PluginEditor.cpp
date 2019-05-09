/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JuceDelayAudioProcessorEditor::JuceDelayAudioProcessorEditor (JuceDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
//    This code is clever it takes the things from the header file
    const OwnedArray<AudioProcessorParameter>& parameters = processor.getParameters();
    int comboBoxCounter = 0;
    
    int editorHeight = 2 * editorMargin;
    for (int i = 0; i < parameters.size(); ++i) {
        if (const AudioProcessorParameterWithID* parameter =
            dynamic_cast<AudioProcessorParameterWithID*> (parameters[i])) {
            
            if (processor.parameters.parameterTypes[i] == "Slider") {
                Slider* aSlider;
                sliders.add (aSlider = new Slider());
                aSlider->setTextValueSuffix (parameter->label);
                aSlider->setTextBoxStyle (Slider::TextBoxLeft,
                                          false,
                                          sliderTextEntryBoxWidth,
                                          sliderTextEntryBoxHeight);
                
                SliderAttachment* aSliderAttachment;
                sliderAttachments.add (aSliderAttachment =
                                       new SliderAttachment (processor.parameters.valueTreeState, parameter->paramID, *aSlider));
                
                components.add (aSlider);
                editorHeight += sliderHeight;
            }
            
            //======================================
            
            else if (processor.parameters.parameterTypes[i] == "ToggleButton") {
                ToggleButton* aButton;
                toggles.add (aButton = new ToggleButton());
                aButton->setToggleState (parameter->getDefaultValue(), dontSendNotification);
                
                ButtonAttachment* aButtonAttachment;
                buttonAttachments.add (aButtonAttachment =
                                       new ButtonAttachment (processor.parameters.valueTreeState, parameter->paramID, *aButton));
                
                components.add (aButton);
                editorHeight += buttonHeight;
            }
            
            //======================================
            
            else if (processor.parameters.parameterTypes[i] == "ComboBox") {
                ComboBox* aComboBox;
                comboBoxes.add (aComboBox = new ComboBox());
                aComboBox->setEditableText (false);
                aComboBox->setJustificationType (Justification::left);
                aComboBox->addItemList (processor.parameters.comboBoxItemLists[comboBoxCounter++], 1);
                
                ComboBoxAttachment* aComboBoxAttachment;
                comboBoxAttachments.add (aComboBoxAttachment =
                                         new ComboBoxAttachment (processor.parameters.valueTreeState, parameter->paramID, *aComboBox));
                
                components.add (aComboBox);
                editorHeight += comboBoxHeight;
            }
            
            //======================================
            
            Label* aLabel;
            labels.add (aLabel = new Label (parameter->name, parameter->name));
            aLabel->attachToComponent (components.getLast(), true);
            addAndMakeVisible (aLabel);
            
            components.getLast()->setName (parameter->name);
            components.getLast()->setComponentID (parameter->paramID);
            addAndMakeVisible (components.getLast());
        }
    }
    
    //======================================
    
    editorHeight += components.size() * editorPadding;
    setSize (editorWidth, editorHeight);
}

JuceDelayAudioProcessorEditor::~JuceDelayAudioProcessorEditor()
{
}

//==============================================================================
void JuceDelayAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

}

void JuceDelayAudioProcessorEditor::resized()
{
    Rectangle<int> r = getLocalBounds().reduced (editorMargin);
    r = r.removeFromRight (r.getWidth() - labelWidth);
    
    for (int i = 0; i < components.size(); ++i) {
        if (Slider* aSlider = dynamic_cast<Slider*> (components[i]))
            components[i]->setBounds (r.removeFromTop (sliderHeight));
        
        if (ToggleButton* aButton = dynamic_cast<ToggleButton*> (components[i]))
            components[i]->setBounds (r.removeFromTop (buttonHeight));
        
        if (ComboBox* aComboBox = dynamic_cast<ComboBox*> (components[i]))
            components[i]->setBounds (r.removeFromTop (comboBoxHeight));
        
        r = r.removeFromBottom (r.getHeight() - editorPadding);
    }
}
