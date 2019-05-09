/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluginParameter.h"

//==============================================================================
JuceDelayAudioProcessor::JuceDelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    parameters (*this)
    , paramDelayTime (parameters, "Delay time", "s", 0.0f, 5.0f, 0.1f)
    , paramFeedback (parameters, "Feedback", "", 0.0f, 0.9f, 0.7f)
    , paramDepth (parameters, "Depth", "", 0.0f, 1.0f, 0.5f)
    , paramFrequency (parameters, "Carrier frequency", "Hz", 0.0f, 5000.0f, 200.0f)
    , paramWaveform (parameters, "Carrier waveform", waveformItemsUI, waveformSine)
    , paramMix (parameters, "Mix", "", 0.0f, 1.0f, 1.0f)
{
    parameters.valueTreeState.state = ValueTree (Identifier (getName().removeCharacters ("- ")));
    
}

JuceDelayAudioProcessor::~JuceDelayAudioProcessor()
{
}

//==============================================================================
const String JuceDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JuceDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JuceDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JuceDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double JuceDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JuceDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int JuceDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JuceDelayAudioProcessor::setCurrentProgram (int index)
{
}

const String JuceDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void JuceDelayAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void JuceDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    const double smoothTime = 1e-3;
    paramDepth.reset (sampleRate, smoothTime);
    paramFrequency.reset (sampleRate, smoothTime);
    paramWaveform.reset (sampleRate, smoothTime);
    
    //======================================
    
    lfoPhase = 0.0f;
    inverseSampleRate = 1.0f / (float)sampleRate;
    twoPi = 2.0f * M_PI;
    
    
    paramDelayTime.reset (sampleRate, smoothTime);
    paramFeedback.reset (sampleRate, smoothTime);
    paramMix.reset (sampleRate, smoothTime);
    
    //======================================
    
    float maxDelayTime = paramDelayTime.maxValue;
    delayBufferSamples = (int)(maxDelayTime * (float)sampleRate) + 1;
    if (delayBufferSamples < 1)
        delayBufferSamples = 1;
    
    delayBufferChannels = getTotalNumInputChannels();
    delayBuffer.setSize (delayBufferChannels, delayBufferSamples);
    delayBuffer.clear();
    
    delayWritePosition = 0;
}

void JuceDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JuceDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void JuceDelayAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    
    const int numInputChannels = getTotalNumInputChannels();
    const int numOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();
    
    //======================================
    
    float currentDelayTime = paramDelayTime.getTargetValue() * (float)getSampleRate();
    float currentFeedback = paramFeedback.getNextValue();
    float currentMix = paramMix.getNextValue();
    
    int localWritePosition;
    
    float currentDepth = paramDepth.getNextValue();
    float currentFrequency = paramFrequency.getNextValue();
    float phase;
    
    for (int channel = 0; channel < numInputChannels; ++channel) {
        float* channelData = buffer.getWritePointer (channel);
        float* delayData = delayBuffer.getWritePointer (channel);
        localWritePosition = delayWritePosition;
        phase = lfoPhase;
        
        
        for (int sample = 0; sample < numSamples; ++sample) {
            const float in = channelData[sample];
            float out = 0.0f;
            
            float readPosition =
            fmodf ((float)localWritePosition - currentDelayTime + (float)delayBufferSamples, delayBufferSamples);
            int localReadPosition = floorf (readPosition);
            
            if (localReadPosition != localWritePosition) {
                float fraction = readPosition - (float)localReadPosition;
                float delayed1 = delayData[(localReadPosition + 0)];
                float delayed2 = delayData[(localReadPosition + 1) % delayBufferSamples];
                float carrier = 2.0f * lfo (phase, (int)paramWaveform.getTargetValue()) - 1.0f;
                
                out = (delayed1 + fraction * (delayed2 - delayed1))*(1 - currentDepth + currentDepth * carrier) ;
                
                channelData[sample] = in + currentMix * (out - in);
                delayData[localWritePosition] = in + out * currentFeedback;
            }
            
            phase += currentFrequency * inverseSampleRate;
            if (phase >= 1.0f)
                phase -= 1.0f;
            
            if (++localWritePosition >= delayBufferSamples)
                localWritePosition -= delayBufferSamples;
        }
    
    }
    
    
    lfoPhase = phase;
    delayWritePosition = localWritePosition;
    
    //======================================
    
    for (int channel = numInputChannels; channel < numOutputChannels; ++channel)
        buffer.clear (channel, 0, numSamples);
}


float JuceDelayAudioProcessor::lfo (float phase, int waveform)
{
    float out = 0.0f;
    
    switch (waveform) {
        case waveformSine: {
            out = 0.5f + 0.5f * sinf (twoPi * phase);
            break;
        }
        case waveformTriangle: {
            if (phase < 0.25f)
                out = 0.5f + 2.0f * phase;
            else if (phase < 0.75f)
                out = 1.0f - 2.0f * (phase - 0.25f);
            else
                out = 2.0f * (phase - 0.75f);
            break;
        }
        case waveformSawtooth: {
            if (phase < 0.5f)
                out = 0.5f + phase;
            else
                out = phase - 0.5f;
            break;
        }
        case waveformInverseSawtooth: {
            if (phase < 0.5f)
                out = 0.5f - phase;
            else
                out = 1.5f - phase;
            break;
        }
        case waveformSquare: {
            if (phase < 0.5f)
                out = 0.0f;
            else
                out = 1.0f;
            break;
        }
        case waveformSquareSlopedEdges: {
            if (phase < 0.48f)
                out = 1.0f;
            else if (phase < 0.5f)
                out = 1.0f - 50.0f * (phase - 0.48f);
            else if (phase < 0.98f)
                out = 0.0f;
            else
                out = 50.0f * (phase - 0.98f);
            break;
        }
    }
    
    return out;
}


//==============================================================================
bool JuceDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* JuceDelayAudioProcessor::createEditor()
{
    return new JuceDelayAudioProcessorEditor (*this);
}

//==============================================================================
void JuceDelayAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void JuceDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JuceDelayAudioProcessor();
}
