#ifndef Piayes_Synth_h
#define Piayes_Synth_h

#include "../JuceLibraryCode/JuceHeader.h"


// a value based on what, if any, chord feature is active
// 0 = no chord feature active, 1 = major
struct NoteData{
    String note;
    double timeStart;
    double timeEnd;
	int note_integer;
	int chordType;
};

extern int chordValue;
extern bool is_combine_button;
extern std::vector<NoteData> bufferOut;

extern bool drum;
extern bool bass;
extern bool piano;



struct SineWaveSound : public SynthesiserSound {
    SineWaveSound();
    
    bool appliesToNote (int /*midiNoteNumber*/) override;
    bool appliesToChannel (int /*midiChannel*/) override;
};


//==============================================================================
/** Our demo synth voice just plays a sine wave.. */
struct SineWaveVoice  : public SynthesiserVoice {
    SineWaveVoice();
    
    bool canPlaySound (SynthesiserSound* sound) override;
    
    void startNote (int midiNoteNumber, float velocity,
                    SynthesiserSound*, int /*currentPitchWheelPosition*/) override;
    
    void stopNote (float /*velocity*/, bool allowTailOff) override;
    
    void pitchWheelMoved (int /*newValue*/) override;
    
    void controllerMoved (int /*controllerNumber*/, int /*newValue*/) override;
    
    void renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override;
    
    
private:
    double currentAngle, angleDelta, level, tailOff;
};


//==============================================================================
// This is an audio source that streams the output of our demo synth.
struct SynthAudioSource  : public AudioSource {
    SynthAudioSource (MidiKeyboardState& keyState);
    
    void setUsingSineWaveSound();
    
    void setUsingSampledSound();
    
    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override;
    
    void releaseResources() override;
    
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    //==============================================================================
    // this collects real-time midi messages from the midi input device, and
    // turns them into blocks that we can process in our audio callback
    MidiMessageCollector midiCollector;
    
    // this represents the state of which keys on our on-screen keyboard are held
    // down. When the mouse is clicked on the keyboard component, this object also
    // generates midi messages for this, which we can pass on to our synth.
    MidiKeyboardState& keyboardState;
    
    // the synth itself!
    Synthesiser synth;
};


#endif
