#include "Synth.h"
#include "CoreFoundation/CoreFoundation.h"
#include <iostream>


SineWaveSound::SineWaveSound() {}


bool SineWaveSound::appliesToNote (int /*midiNoteNumber*/) {
    return true;
}


bool SineWaveSound::appliesToChannel (int /*midiChannel*/) {
    return true;
}


//==============================================================================
SineWaveVoice::SineWaveVoice()   : currentAngle (0), angleDelta (0), level (0), tailOff (0) {}


bool SineWaveVoice::canPlaySound (SynthesiserSound* sound) {
    return dynamic_cast<SineWaveSound*> (sound) != nullptr;
}


void SineWaveVoice::startNote (int midiNoteNumber, float velocity, SynthesiserSound*, int /*currentPitchWheelPosition*/) {
    currentAngle = 0.0;
    level = velocity * 0.15;
    tailOff = 0.0;
    
    double cyclesPerSecond = MidiMessage::getMidiNoteInHertz (midiNoteNumber);
    double cyclesPerSample = cyclesPerSecond / getSampleRate();
    
    angleDelta = cyclesPerSample * 2.0 * double_Pi;
}


void SineWaveVoice::stopNote (float /*velocity*/, bool allowTailOff) {
    if (allowTailOff) {
        if (tailOff == 0.0)
            tailOff = 1.0;
    }
    else {
        clearCurrentNote();
        angleDelta = 0.0;
    }
}


void SineWaveVoice::pitchWheelMoved (int /*newValue*/) {}
void SineWaveVoice::controllerMoved (int /*controllerNumber*/, int /*newValue*/) {}


void SineWaveVoice::renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples) {
    int count = 0;
    
    if (angleDelta != 0.0) {
        if (tailOff > 0) {
            while (--numSamples >= 0) {
                const float currentSample = (float) (std::sin (currentAngle) * level * tailOff);
                
                for (int i = outputBuffer.getNumChannels(); --i >= 0;) {
                    outputBuffer.addSample (i, startSample, currentSample);
                }
                
                currentAngle += angleDelta;
                ++startSample;
                
                tailOff *= 0.99;
                count++;
                if (tailOff <= 0.005) {
                    clearCurrentNote();
                    
                    angleDelta = 0.0;
                    break;
                }
            }
        }
        else {
            while (--numSamples >= 0) {
                const float currentSample = (float) (std::sin (currentAngle) * level);
                
                for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                    outputBuffer.addSample (i, startSample, currentSample);
                
                currentAngle += angleDelta;
                ++startSample;
            }
        }
    }
}


//==============================================================================
// This is an audio source that streams the output of our demo synth.
SynthAudioSource::SynthAudioSource (MidiKeyboardState& keyState)  : keyboardState (keyState) {
    // Add some voices to our synth, to play the sounds..
    for (int i = 4; --i >= 0;) {
        synth.addVoice (new SineWaveVoice());   // These voices will play our custom sine-wave sounds..
        synth.addVoice (new SamplerVoice());
    }
    
    // ..and add a sound for them to play...
    setUsingSineWaveSound();
}


void SynthAudioSource::setUsingSineWaveSound() {
    synth.clearSounds();
    synth.addSound (new SineWaveSound());
}


void SynthAudioSource::setUsingSampledSound() {
    WavAudioFormat wavFormat;
    
    //Gets working directory in order for Archived file to find the path of the
    //.wav files
    
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
    {
        // error!
    }
    CFRelease(resourcesURL);
    
    chdir(path);
    //std::cout << "Current Path: " << path << std::endl;
    
    std::string pathName = path;
    
    
    if (drum == true) {
//        std::cout << File::getCurrentWorkingDirectory().getFileName() << std::endl;
//        
        char wd[1024];
        std::cout << getcwd(wd, sizeof(wd)) << std::endl;

        FileInputStream* sound = new FileInputStream (File(pathName + "/Electro-Tom.wav"));
        
        if (sound->openedOk()) {
            ScopedPointer<AudioFormatReader> audioReader (wavFormat.createReaderFor (sound,true));
            
            BigInteger allNotes;
            allNotes.setRange (0, 128, true);
            
            synth.clearSounds();
            synth.addSound (new SamplerSound ("demo sound",
                                              *audioReader,
                                              allNotes,
                                              74,   // root midi note
                                              0.1,  // attack time
                                              0.1,  // release time
                                              10.0  // maximum sample length
                                              ));
        }
        else{
            std::cout << "open failure" << std::endl;
        }
    }
    else if (bass == true) {
        FileInputStream* sound = new FileInputStream (File(pathName + "/bass.wav"));
        
        if (sound->openedOk()) {
            ScopedPointer<AudioFormatReader> audioReader (wavFormat.createReaderFor (sound,true));
            
            BigInteger allNotes;
            allNotes.setRange (0, 128, true);
            
            synth.clearSounds();
            synth.addSound (new SamplerSound ("demo sound",
                                              *audioReader,
                                              allNotes,
                                              74,   // root midi note
                                              0.1,  // attack time
                                              0.1,  // release time
                                              10.0  // maximum sample length
                                              ));
        }
    }
    else if (piano == true) {
        FileInputStream* sound = new FileInputStream (File(pathName + "/piano.wav"));
        
        if (sound->openedOk()) {
            ScopedPointer<AudioFormatReader> audioReader (wavFormat.createReaderFor (sound,true));
            
            BigInteger allNotes;
            allNotes.setRange (0, 128, true);
            
            synth.clearSounds();
            synth.addSound (new SamplerSound ("demo sound",
                                              *audioReader,
                                              allNotes,
                                              74,   // root midi note
                                              0.1,  // attack time
                                              0.1,  // release time
                                              10.0  // maximum sample length
                                              ));
        }
    }
}


void SynthAudioSource::prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) {
    midiCollector.reset (sampleRate);
    
    synth.setCurrentPlaybackSampleRate (sampleRate);
}


void SynthAudioSource::releaseResources() {}


void SynthAudioSource::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) {
    // the synth always adds its output to the audio buffer, so we have to clear it first..
    bufferToFill.clearActiveBufferRegion();
    
    // fill a midi buffer with incoming messages from the midi input.
    MidiBuffer incomingMidi;
    midiCollector.removeNextBlockOfMessages (incomingMidi, bufferToFill.numSamples);
    
    // pass these messages to the keyboard state so that it can update the component
    // to show on-screen which keys are being pressed on the physical midi keyboard.
    // This call will also add midi messages to the buffer which were generated by
    // the mouse-clicking on the on-screen keyboard.
    if (chordValue) {
        // if the chord feature is active run the overloaded version pertaining to that
        keyboardState.processNextMidiBuffer (incomingMidi, 0, bufferToFill.numSamples, true, chordValue);
    }
    else if (!is_combine_button) {
        keyboardState.processNextMidiBuffer (incomingMidi, 0, bufferToFill.numSamples, true);
    }
    else {
        //        keyboardState.processNextMidiBuffer (incomingMidi, 0, bufferToFill.numSamples, bufferOut, true);
    }
    
    // and now get the synth to process the midi events and generate its output.
    synth.renderNextBlock (*bufferToFill.buffer, incomingMidi, 0, bufferToFill.numSamples);
}
