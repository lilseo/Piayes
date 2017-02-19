#include "MainComponent.h"


MainContentComponent::MainContentComponent()
: deviceManager (getSharedAudioDeviceManager()),
lastInputIndex (0),
isAddingFromMidiInput (false),
keyboardComponent (keyboardState, MidiKeyboardComponent::horizontalKeyboard),
startTime (Time::getMillisecondCounterHiRes() * 0.001) {
    
    
    // MIDI Inputs
    const StringArray midiInputs (MidiInput::getDevices());
    midiInputList.addItemList (midiInputs, 1);
    midiInputList.addListener (this);
    
    // Find the first enabled device and use that by default
    for (int i = 0; i < midiInputs.size(); ++i)
    {
        if (deviceManager.isMidiInputEnabled (midiInputs[i]))
        {
            setMidiInput (i);
            break;
        }
    }
    
    // If no enabled devices were found just use the first one in the list
    if (midiInputList.getSelectedId() == 0) {
        setMidiInput (0);
    }
    
    // MIDI Outputs
    midiOutputList.addItemList (MidiOutput::getDevices(), 1);
    midiOutputList.addListener (this);
    
    addAndMakeVisible (keyboardComponent);
    keyboardState.addListener (this);
    
    // Controls
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
    
    addAndMakeVisible (clearButton);
    clearButton.setButtonText ("Clear Recording");
    clearButton.addListener (this);
    
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

MainContentComponent::~MainContentComponent() {
    keyboardState.removeListener (this);
    audioSourcePlayer.setSource (nullptr);
    deviceManager.removeAudioCallback (&audioSourcePlayer);
}

void MainContentComponent::resized() {
    Rectangle<int> area (getLocalBounds());
    keyboardComponent.setBounds (area.removeFromTop (80).reduced(8));
    
    recordButton.setBounds (16, 100, 150, 24);
    stopRecordButton.setBounds (16, 125, 150, 24);
    playNotesButton.setBounds (16, 150, 150, 24);
    combineButton.setBounds (16, 175, 150, 24);
    clearButton.setBounds (16, 200, 150, 24);
    notesButton.setBounds (16, 225, 150, 24);
    rhythmButton.setBounds (16, 250, 150, 24);
}


AudioDeviceManager& MainContentComponent::getSharedAudioDeviceManager() {
    if (sharedAudioDeviceManager == nullptr)
    {
        sharedAudioDeviceManager = new AudioDeviceManager();
        RuntimePermissions::request (RuntimePermissions::recordAudio, runtimePermissionsCallback);
    }
    
    return *sharedAudioDeviceManager;
}


/** Starts listening to a MIDI input device, enabling it if necessary. */
void MainContentComponent::setMidiInput (int index)
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

void MainContentComponent::setMidiOutput (int index)
{
    currentMidiOutput = nullptr;
    
    if (MidiOutput::getDevices() [index].isNotEmpty())
    {
        currentMidiOutput = MidiOutput::openDevice (index);
        jassert (currentMidiOutput);
    }
}


void MainContentComponent::comboBoxChanged (ComboBox* box)
{
    if (box == &midiInputList) {
        std::cout << &midiInputList << std::endl;
        setMidiInput  (midiInputList.getSelectedItemIndex());
    }
    if (box == &midiOutputList) {
        setMidiOutput (midiOutputList.getSelectedItemIndex());
    }
}


// Handle callbacks from the midi device and on-screen keyboard
void MainContentComponent::handleIncomingMidiMessage (MidiInput* source, const MidiMessage& message) {
    const ScopedValueSetter<bool> scopedInputFlag (isAddingFromMidiInput, true);
    keyboardState.processNextMidiEvent (message);
}


void MainContentComponent::handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) {
    MidiMessage m (MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity));
    m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
    
    if (record and setNotes) {
        std::cout << MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3) << std::endl;
        notes.push_back(MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3));
        bufferNotes = notes;
        std::cout << "Notes: ";
        for (String n : bufferNotes)
            std::cout << n << ' ';
        std::cout << std::endl;
    }
    
    const double time = m.getTimeStamp() - startTime;
    // Not used as of now
    const int hours = ((int) (time / 3600.0)) % 24;
    const int minutes = ((int) (time / 60.0)) % 60;
    const int seconds = ((int) time) % 60;
    const int millis = ((int) (time * 1000.0)) % 1000;
    const String timecode (String::formatted ("%02d:%02d:%02d.%03d", hours, minutes, seconds, millis));
    
    if (record and setRhythm) {
        times.push_back(time);
        std::cout << timecode << std::endl;
        bufferTimes = times;
    }
}


void MainContentComponent::handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float /*velocity*/) {
    MidiMessage m (MidiMessage::noteOff (midiChannel, midiNoteNumber));
    m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
    const double time = m.getTimeStamp() - startTime;
    // For now: just use time instead of timecode, need to edit times as doubles later
    const int hours = ((int) (time / 3600.0)) % 24;
    const int minutes = ((int) (time / 60.0)) % 60;
    const int seconds = ((int) time) % 60;
    const int millis = ((int) (time * 1000.0)) % 1000;
    const String timecode (String::formatted ("%02d:%02d:%02d.%03d", hours, minutes, seconds, millis));
    
    if (record and setRhythm) {
        times.push_back(time);
        std::cout << timecode << std::endl;
        bufferTimes = times;
    }
}


MainContentComponent::IncomingMessageCallback::IncomingMessageCallback (MainContentComponent* o, const MidiMessage& m, const String& s)
: owner (o), message (m), source (s)
{}

void MainContentComponent::playNotes () {
    for (int i = 0; i < bufferNotes.size(); i++) {
        std::cout << bufferNotes[i] << " : " << convertNameToMidi(bufferNotes[i]) << std::endl;
        notesMidi.push_back(convertNameToMidi(bufferNotes[i]));
    }
}

std::vector<NoteData> MainContentComponent::combineData(std::vector<String> notes, std::vector<double> times){
    std::vector<NoteData> bufferOut;
    std::cout << "size of notes: "<< notes.size() << std::endl << "size of times:" << times.size() << std::endl;
    if (notes.empty() || times.empty()){
        std::cout << "Record (1) Notes and (2) Tempo before combining tracks!" << std::endl;
        return bufferOut; // Will be empty... TODO: handle this a better way
    }
    NoteData newNote;
    double timeZero = times[0]; // timeStart of first note, subtracted from all subsequent times
                                // Leave a few milliseconds of buffer?
    
    for (int i = 0; i < notes.size(); ++i){
        newNote.note = notes[i];
        if (i < (times.size() / 2)){
            newNote.timeStart = times[i * 2] - timeZero;
            newNote.timeEnd = times[(i * 2) + 1] - timeZero;
        }
        // Special case: user does not input rhythm for all notes
        // Default: set note to start 1.0 sec after last note ended, duration 0.5 seconds
        else{
            double lastNoteEnd = bufferOut[bufferOut.size()-1].timeEnd;
            newNote.timeStart = lastNoteEnd + 1.0;
            newNote.timeEnd = newNote.timeStart + 0.5;
        }
        bufferOut.push_back(newNote);
    } // for (all notes recorded)
    return bufferOut;
}


int MainContentComponent::convertNameToMidi (String noteString) {
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
        return 13*octave + base;
    }
}


void MainContentComponent::buttonClicked (Button* buttonThatWasClicked) {
   // std::cout << "record: " << record << "  set notes: " << setNotes << "  set rhythm: " << setRhythm << std::endl;
    if (setNotes == true and setRhythm == true)
        std::cout << "setNotes and setRhythm both true" << std::endl;
    
    if (buttonThatWasClicked == &recordButton and record == false) {
        if (setNotes == false and setRhythm == false)
            std::cout << "Select either \"Set notes \" or \"Set rhythym\" before recording" << std::endl;
        else
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
    else if (buttonThatWasClicked == &clearButton){
        bufferNotes.clear();
        bufferTimes.clear();
        record = false;
    }
}

void MainContentComponent::runtimePermissionsCallback (bool wasGranted) {
    int numInputChannels = wasGranted ? 2 : 0;
    sharedAudioDeviceManager->initialise (numInputChannels, 2, nullptr, true, String(), nullptr);
}

Component* createMainContentComponent()     { return new MainContentComponent(); }
