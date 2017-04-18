//
//  MainComponent.h
//  Piayes

#include "../JuceLibraryCode/JuceHeader.h"
#include "CustomLookAndFeel.h"
#include "BinaryData.h"
#include "Synth.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


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
std::vector<std::vector<NoteData>> bufferVectorNotes;
std::vector<double> bufferTimes;
MidiBuffer outputBuffer; 
MidiMessage newNote;
bool edit = false;
bool drum = false;
bool bass = false;
bool piano = false;
bool stopRecordingFromController = false;
int tempoCounter;


int chordValue = 0;
bool is_combine_button = false;

class MainContentComponent  :   public Component,
                                private ComboBox::Listener,
                                private MidiInputCallback,
                                private MidiKeyboardStateListener,
                                private Slider::Listener,
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
                                    
    CustomLookAndFeel customFeel;
    
    TextEditor notesBox;
    TextEditor rhythmBox;
    TextEditor feedbackBox;
                                    
    MidiKeyboardState keyboardState;
    MidiKeyboardComponent keyboardComponent;
    
    TextButton recordButton;
    TextButton stopRecordButton;
    TextButton playNotesButton;
    TextButton combineButton;
                                    TextButton editNote;
    TextButton clearButton;
    ToggleButton notesButton;
    ToggleButton rhythmButton;
    ToggleButton singleNoteButton;
    ToggleButton chordMajorButton;
    ToggleButton chordMinorButton;
	ToggleButton chordSeventhMajorButton;
	ToggleButton chordSeventhDominantButton;
	ToggleButton chordSeventhMinorButton;
	
                                    
    bool record = false;
    bool setNotes = false;
    bool setRhythm = false;
	bool midiRecordButton = false;
    
    ToggleButton sineButton;
    ToggleButton drumButton;
    ToggleButton bassButton;
    ToggleButton pianoButton;
                                    
    ToggleButton nativeButton;                                
    TextButton saveButton;
    TextButton loadButton;
    
    ShapeButton volumeUpButton;
    ShapeButton volumeDownButton;
                                    
    TextButton tutorialButton;
                                    
    void setButton(TextButton* button, String text);
	
	Label feedbackLabel;
	Label instrumentsLabel;
	Label chordsLabel;
	Label editingLabel;
	Label MemoryLabel;
	Label melodyRhythm;
    Label volumeSliderLabel;
    Label volumeUpLabel;
    Label volumeDownLabel;
    
    Slider volumeSlider;

                                    
    ComboBox midiInputList, midiOutputList;
    ScopedPointer<MidiOutput> currentMidiOutput;
    
    SynthAudioSource synthAudioSource;
    
    std::vector<NoteData> notes;
    std::vector<std::vector<NoteData>> notesVectors;
    std::vector<double> times;
	std::vector<MidiMessage> MidiRhythm;
    std::vector<int> notesMidi;
    std::vector<MidiMessage> temp;
    
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
    
    void handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity);
    
    void handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*velocity*/) override;
    
    //void postMessageToList (const MidiMessage& message, const String& source);
     
    //void addMessageToList (const MidiMessage& message, const String& source);
    
    void playNotes (std::vector<MidiMessage> temp);
    
    void combineData(std::vector<NoteData> notes, std::vector<MidiMessage> MidiRhythm);
    
    int convertNameToMidi (String noteString);
    
    void buttonClicked (Button* buttonThatWasClicked);
        
    static void runtimePermissionsCallback (bool wasGranted);
    
    void sliderValueChanged(Slider* slider) override;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent);
};
