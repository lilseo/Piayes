//
//  MainComponent.h
//  Piayes

#include "../JuceLibraryCode/JuceHeader.h"
#include "BinaryData.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


bool is_combine_button = false;

struct SineWaveSound : public SynthesiserSound
{
    SineWaveSound();

    bool appliesToNote (int /*midiNoteNumber*/) override;
    bool appliesToChannel (int /*midiChannel*/) override;
};


//==============================================================================
/** Our demo synth voice just plays a sine wave.. */
struct SineWaveVoice  : public SynthesiserVoice
{
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
struct SynthAudioSource  : public AudioSource
{
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


static ScopedPointer<AudioDeviceManager> sharedAudioDeviceManager;
// Data structure to hold relevant note data for final output buffer
/*struct NoteData{
    String note;
    double timeStart;
    double timeEnd;
};*/

// Global vectors to take hold buffer data
std::vector<NoteData> bufferOut;
std::vector<NoteData> bufferNotes;
std::vector<double> bufferTimes;

class MainContentComponent  :   public Component,
                                private ComboBox::Listener,
                                private MidiInputCallback,
                                private MidiKeyboardStateListener,
                                private Button::Listener {
public:
    MainContentComponent();
    
    ~MainContentComponent();
    
    void resized() override;
    
    
private:
    static AudioDeviceManager& getSharedAudioDeviceManager();
    AudioDeviceManager& deviceManager;
    AudioSourcePlayer audioSourcePlayer;
    int lastInputIndex;
    bool isAddingFromMidiInput;
    double startTime;
    
    TextEditor notesBox;
    TextEditor rhythmBox;
    TextEditor feedbackBox;
                                    
    MidiKeyboardState keyboardState;
    MidiKeyboardComponent keyboardComponent;
    
    TextButton recordButton;
    TextButton stopRecordButton;
    TextButton playNotesButton;
    TextButton combineButton;
    TextButton clearButton;
    ToggleButton notesButton;
    ToggleButton rhythmButton;
    bool record = false;
    bool setNotes = false;
    bool setRhythm = false;
                         
    ToggleButton nativeButton;                                
    TextButton saveButton;
    TextButton loadButton;
                                    
    ComboBox midiInputList, midiOutputList;
    ScopedPointer<MidiOutput> currentMidiOutput;
    
    SynthAudioSource synthAudioSource;

    
    std::vector<NoteData> notes;
    std::vector<double> times;
    std::vector<int> notesMidi;
    
    class IncomingMessageCallback   : public CallbackMessage {
      public:
        IncomingMessageCallback (MainContentComponent* o, const MidiMessage& m, const String& s);
        Component::SafePointer<MainContentComponent> owner;
        MidiMessage message;
        String source;
    };

    
    /** Starts listening to a MIDI input device, enabling it if necessary. */
    void setMidiInput (int index);
    
    void setMidiOutput (int index);

    void comboBoxChanged (ComboBox* box) override;
                                    
    void logNoteMessage (const String& m);
                                    
    void logRhythmMessage (const String& m);
                                    
    void logFeedback (const String& m);

    // Handle callbacks from the midi device and on-screen keyboard
    void handleIncomingMidiMessage (MidiInput* source, const MidiMessage& message) override;
    
    void handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    
    void handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*velocity*/) override;
    
    //void postMessageToList (const MidiMessage& message, const String& source);
     
    //void addMessageToList (const MidiMessage& message, const String& source);
    
    void playNotes ();
    
    std::vector<NoteData> combineData(std::vector<NoteData> notes, std::vector<double> times);
    
    int convertNameToMidi (String noteString);
    
    void buttonClicked (Button* buttonThatWasClicked);
        
    static void runtimePermissionsCallback (bool wasGranted);
    
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent);
};
