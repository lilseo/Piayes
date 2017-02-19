#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

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
    MainContentComponent()
    : deviceManager (getSharedAudioDeviceManager()),
    lastInputIndex (0),
    isAddingFromMidiInput (false),
    keyboardComponent (keyboardState, MidiKeyboardComponent::horizontalKeyboard),
    startTime (Time::getMillisecondCounterHiRes() * 0.001) {
        
        
        // MIDI Inputs
        const StringArray midiInputs (MidiInput::getDevices());
        midiInputList.addItemList (midiInputs, 1);
        midiInputList.addListener (this);
        
        // find the first enabled device and use that by default
        for (int i = 0; i < midiInputs.size(); ++i)
        {
            if (deviceManager.isMidiInputEnabled (midiInputs[i]))
            {
                setMidiInput (i);
                break;
            }
        }
        
        // if no enabled devices were found just use the first one in the list
        if (midiInputList.getSelectedId() == 0) {
            setMidiInput (0);
        }
        
        // MIDI Outputs
        midiOutputList.addItemList (MidiOutput::getDevices(), 1);
        midiOutputList.addListener (this);
        
        addAndMakeVisible (keyboardComponent);
        keyboardState.addListener (this);
        
        // YELENA
        addAndMakeVisible (keyboardComponent);
        keyboardState.addListener (this);
        
        addAndMakeVisible (recordButton);
        recordButton.setButtonText ("Record");
        recordButton.addListener (this);
        
        addAndMakeVisible (stopRecordButton);
        stopRecordButton.setButtonText ("Stop Recording");
        stopRecordButton.addListener (this);
        
        addAndMakeVisible (playNotesButton);
        playNotesButton.setButtonText ("Playback Notes");
        playNotesButton.addListener (this);
        
        addAndMakeVisible (combineButton);
        combineButton.setButtonText ("Combine Tracks");
        combineButton.addListener (this);
        
        addAndMakeVisible (notesButton);
        notesButton.setButtonText ("Set notes");
        notesButton.setRadioGroupId (1);
        notesButton.addListener (this);
        
        addAndMakeVisible (rhythmButton);
        rhythmButton.setButtonText ("Set rhythm");
        rhythmButton.setRadioGroupId (1);
        rhythmButton.addListener (this);
        
        
        setSize (600, 400);
    }
    
    ~MainContentComponent() {
        keyboardState.removeListener (this);
        audioSourcePlayer.setSource (nullptr);
        deviceManager.removeAudioCallback (&audioSourcePlayer);
    }
    
    void resized() override {
        Rectangle<int> area (getLocalBounds());
        keyboardComponent.setBounds (area.removeFromTop (80).reduced(8));
        
        recordButton.setBounds (16, 100, 150, 24);
        stopRecordButton.setBounds (16, 125, 150, 24);
        playNotesButton.setBounds (16, 150, 150, 24);
        combineButton.setBounds (16, 175, 150, 24);
        notesButton.setBounds (16, 200, 150, 24);
        rhythmButton.setBounds (16, 225, 150, 24);
    }
    
    
private:
    AudioDeviceManager& deviceManager;
    AudioSourcePlayer audioSourcePlayer;
    int lastInputIndex;
    bool isAddingFromMidiInput;
    double startTime;
    
    MidiKeyboardState keyboardState;
    MidiKeyboardComponent keyboardComponent;
    
    TextButton recordButton;
    TextButton stopRecordButton;
    TextButton playNotesButton;
    TextButton combineButton;
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
    
    
    static AudioDeviceManager& getSharedAudioDeviceManager() {
        if (sharedAudioDeviceManager == nullptr)
        {
            sharedAudioDeviceManager = new AudioDeviceManager();
            RuntimePermissions::request (RuntimePermissions::recordAudio, runtimePermissionsCallback);
        }
        
        return *sharedAudioDeviceManager;
    }
    
    
    /** Starts listening to a MIDI input device, enabling it if necessary. */
    void setMidiInput (int index)
    {
        const StringArray list (MidiInput::getDevices());
        
        deviceManager.removeMidiInputCallback (list[lastInputIndex], this);
        
        const String newInput (list[index]);
        
        if (! deviceManager.isMidiInputEnabled (newInput)) {
            deviceManager.setMidiInputEnabled (newInput, true);
        }
        
        deviceManager.addMidiInputCallback (newInput, this);
        midiInputList.setSelectedId (index + 1, dontSendNotification);
        
        lastInputIndex = index;
    }
    
    void setMidiOutput (int index)
    {
        currentMidiOutput = nullptr;
        
        if (MidiOutput::getDevices() [index].isNotEmpty())
        {
            currentMidiOutput = MidiOutput::openDevice (index);
            jassert (currentMidiOutput);
        }
    }
    
    
    void comboBoxChanged (ComboBox* box) override
    {
        if (box == &midiInputList) {
            setMidiInput  (midiInputList.getSelectedItemIndex());
        }
        if (box == &midiOutputList) {
            setMidiOutput (midiOutputList.getSelectedItemIndex());
        }
    }
    
    
    // Handle callbacks from the midi device and on-screen keyboard
    void handleIncomingMidiMessage (MidiInput* source, const MidiMessage& message) override {
        const ScopedValueSetter<bool> scopedInputFlag (isAddingFromMidiInput, true);
        keyboardState.processNextMidiEvent (message);
        //std::cout << "message:" << message.getDescription() << std::endl;
        postMessageToList (message, source->getName());
    }
    
    
    void handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override {
        //std::cout << "handleNoteOn" << std::endl;
        MidiMessage m (MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity));
        m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
        
        if (record and setNotes) {
            std::cout << MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3) << std::endl;
            notes.push_back(MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3));
            /*
             std::cout << "Notes so far: ";
             for (int i = 0; i < notes.size(); i++) {
             std::cout << notes[i] << ", ";
             }
             std::cout << std::endl;
             */
        }
        
        postMessageToList (m, "On-Screen Keyboard");
        
        if (record and setNotes){
            bufferNotes = notes;
            std::cout << "Notes: ";
            for (String n : bufferNotes)
                std::cout << n << ' ';
            std::cout << std::endl;
        }
    }
    
    
    void handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*velocity*/) override {
        //std::cout << "handleNoteOff" << std::endl;
        if (! isAddingFromMidiInput) {
            MidiMessage m (MidiMessage::noteOff (midiChannel, midiNoteNumber));
            m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
            postMessageToList (m, "On-Screen Keyboard");
        }
    }
    
    
    class IncomingMessageCallback   : public CallbackMessage {
    public:
        IncomingMessageCallback (MainContentComponent* o, const MidiMessage& m, const String& s)
        : owner (o), message (m), source (s)
        {}
        
        void messageCallback() override {
            if (owner != nullptr)
                owner->addMessageToList (message, source);
                }
        
        Component::SafePointer<MainContentComponent> owner;
        MidiMessage message;
        String source;
    };
    
    
    void postMessageToList (const MidiMessage& message, const String& source) {
        (new IncomingMessageCallback (this, message, source))->post();
    }
    
    
    void addMessageToList (const MidiMessage& message, const String& source) {
        const double time = message.getTimeStamp() - startTime;
        
        const int hours = ((int) (time / 3600.0)) % 24;
        const int minutes = ((int) (time / 60.0)) % 60;
        const int seconds = ((int) time) % 60;
        const int millis = ((int) (time * 1000.0)) % 1000;
        
        const String timecode (String::formatted ("%02d:%02d:%02d.%03d", hours, minutes, seconds, millis));
        
        if (record and setRhythm) {
            // Lily: changed to time, need to edit as doubles later
            times.push_back(time);
            std::cout << timecode << std::endl;
            bufferTimes = times;
        }
    }
    
    
    void playNotes () {
        for (int i = 0; i < notes.size(); i++) {
            std::cout << notes[i] << " : " << convertNameToMidi(notes[i]) << std::endl;
            notesMidi.push_back(convertNameToMidi(notes[i]));
        }
    }
    
    std::vector<NoteData> combineData(std::vector<String> notes, std::vector<double> times){
        std::vector<NoteData> bufferOut;
        std::cout << "size of notes: "<< notes.size() << std::endl << "size of times:" << times.size() << std::endl;
        if (notes.empty() || times.empty()){
            std::cout << "Record (1) Notes and (2) Tempo before combining tracks!" << std::endl;
            return bufferOut; // will be empty; handle this a better way
        }
        
        NoteData newNote;
        for (int i = 0; i < notes.size(); ++i){
            newNote.note = notes[i];
            if (i < (times.size() / 2)){
                newNote.timeStart = times[i * 2];
                newNote.timeEnd = times[(i * 2) + 1];
            }
            // Special case: user does not input rhythm for all notes
            // For now: set note to start 1.0 sec after last note ended, duration 0.5 seconds
            else{
                double lastNoteEnd = bufferOut[bufferOut.size()-1].timeEnd;
                newNote.timeStart = lastNoteEnd + 1.0;
                newNote.timeEnd = newNote.timeStart + 0.5;
            }
            bufferOut.push_back(newNote);
        } // for (all notes recorded)
        return bufferOut;
    }
    
    
    int convertNameToMidi (String noteString) {
        std::string note = noteString.toStdString();
        
        char noteChar = note.at(0);
        char sharpSpot = note.at(1);
        int base = 0;
        
        switch (noteChar) {
            case 'C':
                if (sharpSpot == '#') base = 1;
                break;
            case 'D':
                base = 2;
                if (sharpSpot == '#') base = 3;
                break;
            case 'E':
                base = 4;
                break;
            case 'F':
                base = 5;
                if (sharpSpot == '#') base = 6;
                break;
            case 'G':
                base = 7;
                if (sharpSpot == '#') base = 8;
                break;
            case 'A':
                base = 9;
                if (sharpSpot == '#') base = 10;
                break;
            case 'B':
                base = 11;
                break;
        }
        
        if (note.substr(note.length() - 2) == "-1") {
            return base;
        }
        else {
            int octave = int(note.back()) - 48;
            //std::cout << "note.back(): " << int(note.back()) << std::endl;
            return 13*octave + base;
        }
    }
    
    
    void buttonClicked (Button* buttonThatWasClicked) {
        if (buttonThatWasClicked == &recordButton and record == false) {
            record = true;
        }
        else if (buttonThatWasClicked == &stopRecordButton and record == true) {
            record = false;
        }
        else if (buttonThatWasClicked == &playNotesButton) {
            playNotes();
        }
        else if (buttonThatWasClicked == &notesButton) {
            notes.clear();
            setNotes = true;
            setRhythm = false;
        }
        else if (buttonThatWasClicked == &rhythmButton) {
            times.clear();
            setNotes = false;
            setRhythm = true;
        }
        // Combine pitch and rhythm data
        else if (buttonThatWasClicked == &combineButton){
            bufferOut = combineData(bufferNotes, bufferTimes);
            std::cout << "bufferOut: " << std::endl;
            for (NoteData n : bufferOut)
                std::cout << n.note << ' ' << n.timeStart << ' ' << n.timeEnd << std::endl;
        }
    }
    
    static void runtimePermissionsCallback (bool wasGranted) {
        int numInputChannels = wasGranted ? 2 : 0;
        sharedAudioDeviceManager->initialise (numInputChannels, 2, nullptr, true, String(), nullptr);
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent);
};

Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED

