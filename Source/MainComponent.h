//
//  MainComponent.h
//  Piayes

#include "../JuceLibraryCode/JuceHeader.h"
#include <iostream>
#include <vector>
#include <string>

static ScopedPointer<AudioDeviceManager> sharedAudioDeviceManager;
// Data structure to hold relevant note data for final output buffer
struct NoteData{
    String note;
    double timeStart;
    double timeEnd;
};

// Global vectors to take hold buffer data
std::vector<NoteData> bufferOut;
std::vector<String> bufferNotes;
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
                                    
    ComboBox midiInputList, midiOutputList;
    ScopedPointer<MidiOutput> currentMidiOutput;
	
	std::vector<String> notes;
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
    
	std::vector<NoteData> combineData(std::vector<String> notes, std::vector<double> times);
	
    int convertNameToMidi (String noteString);
    
    void buttonClicked (Button* buttonThatWasClicked);
		
	static void runtimePermissionsCallback (bool wasGranted);
	
		
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent);
};
