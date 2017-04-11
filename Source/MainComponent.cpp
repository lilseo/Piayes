#include "MainComponent.h"
#include "Synth.h"


MainContentComponent::MainContentComponent()
    : deviceManager (getSharedAudioDeviceManager()),
    lastInputIndex (0),
    isAddingFromMidiInput (false),
    synthAudioSource (keyboardState),
    keyboardComponent (keyboardState, MidiKeyboardComponent::horizontalKeyboard),
    startTime (Time::getMillisecondCounterHiRes() * 0.001) {
    
		
    
    // Set the UI Controls
    addAndMakeVisible (keyboardComponent);
    keyboardState.addListener (this);
    
    addAndMakeVisible (notesBox);
    notesBox.setMultiLine (true);
    notesBox.setReadOnly(true);
    logNoteMessage("Notes: ");
    
    addAndMakeVisible (rhythmBox);
    rhythmBox.setMultiLine (true);
    rhythmBox.setReadOnly(true);
    logRhythmMessage("Times: ");
    
    addAndMakeVisible (feedbackBox);
    feedbackBox.setMultiLine (true);
    feedbackBox.setReadOnly(true);
    
    addAndMakeVisible (recordButton);
    setButton(&recordButton, "Record");
    
    addAndMakeVisible (stopRecordButton);
    setButton(&stopRecordButton, "Stop Recording");
    
    addAndMakeVisible (playNotesButton);
    setButton(&playNotesButton, "Playback Notes");
    
    addAndMakeVisible (combineButton);
    setButton(&combineButton, "Combine Tracks");
    
    addAndMakeVisible (clearButton);
    setButton(&clearButton, "Clear Recording");
    
    addAndMakeVisible (notesButton);
    notesButton.setButtonText ("Set Notes");
    notesButton.setRadioGroupId (1);
    notesButton.addListener (this);
    notesButton.setToggleState (true, dontSendNotification);
    setNotes = true;
	notesButton.setColour(juce::ToggleButton::textColourId, juce::Colour(250, 250, 250));
    
    addAndMakeVisible (rhythmButton);
    rhythmButton.setButtonText ("Set Rhythm");
    rhythmButton.setRadioGroupId (1);
    rhythmButton.addListener (this);
	rhythmButton.setColour(juce::ToggleButton::textColourId, juce::Colour(250, 250, 250));
        
    addAndMakeVisible (sineButton);
    sineButton.setButtonText ("Synth");
    sineButton.setRadioGroupId (2);
    sineButton.addListener (this);
    sineButton.setToggleState (true, dontSendNotification);
	sineButton.setColour(juce::ToggleButton::textColourId, juce::Colour(250, 250, 250));
        
    addAndMakeVisible (drumButton);
    drumButton.setButtonText ("Drums");
    drumButton.setRadioGroupId (2);
    drumButton.addListener (this);
	drumButton.setColour(juce::ToggleButton::textColourId, juce::Colour(250, 250, 250));
        
    addAndMakeVisible (bassButton);
    bassButton.setButtonText ("Bass");
    bassButton.setRadioGroupId (2);
    bassButton.addListener (this);
	bassButton.setColour(juce::ToggleButton::textColourId, juce::Colour(250, 250, 250));
    
    addAndMakeVisible (pianoButton);
    pianoButton.setButtonText ("Piano");
    pianoButton.setRadioGroupId (2);
    pianoButton.addListener (this);
    pianoButton.setColour(juce::ToggleButton::textColourId, juce::Colour(250, 250, 250));
        
    addAndMakeVisible (saveButton);
    setButton(&saveButton, "Save");
    
    addAndMakeVisible (loadButton);
    setButton(&loadButton, "Load");
        
    addAndMakeVisible (tutorialButton);
    setButton(&tutorialButton, "Tutorial");

	//Font::bold

	// Titles
	addAndMakeVisible(feedbackLabel);
	feedbackLabel.setText("Real Time Feedback", dontSendNotification);
	feedbackLabel.setColour(juce::Label::textColourId, juce::Colour(255, 218, 45));
	feedbackLabel.setFont(juce::Font(20, bold));

	
	addAndMakeVisible(instrumentsLabel);
	instrumentsLabel.setText("Instruments", dontSendNotification);
	instrumentsLabel.setColour(juce::Label::textColourId, juce::Colour(255, 218, 45));
	instrumentsLabel.setFont(juce::Font(20, bold));

	addAndMakeVisible(chordsLabel);
	chordsLabel.setText("Chords", dontSendNotification);
	chordsLabel.setColour(juce::Label::textColourId, juce::Colour(255, 218, 45));
	chordsLabel.setFont(juce::Font(20, bold));

	addAndMakeVisible(editingLabel);
	editingLabel.setText("Recording & Editing", dontSendNotification);
	editingLabel.setColour(juce::Label::textColourId, juce::Colour(255, 218, 45));
	editingLabel.setFont(juce::Font(20, bold));
	
	addAndMakeVisible(melodyRhythm);
	melodyRhythm.setText("Melody & Rhythm", dontSendNotification);
	melodyRhythm.setColour(juce::Label::textColourId, juce::Colour(255, 218, 45));
	melodyRhythm.setFont(juce::Font(20, bold));

	addAndMakeVisible(MemoryLabel);
	MemoryLabel.setText("Save & Load", dontSendNotification);
	MemoryLabel.setColour(juce::Label::textColourId, juce::Colour(255, 218, 45));
	MemoryLabel.setFont(juce::Font(20, bold));

	
    
    // putting in major chord activator
    addAndMakeVisible (singleNoteButton);
    singleNoteButton.setButtonText("Play A Single Note");
    singleNoteButton.setRadioGroupId(3);
    singleNoteButton.addListener(this);
    singleNoteButton.setToggleState (true, dontSendNotification);
	singleNoteButton.setColour(juce::ToggleButton::textColourId, juce::Colour(250, 250, 250));
	
	//(juce::Font(20, bold));
    
    // putting in major chord activator
    addAndMakeVisible (chordMajorButton);
    chordMajorButton.setButtonText("Play Major Chords");
    chordMajorButton.setRadioGroupId(3);
    chordMajorButton.addListener(this);
	chordMajorButton.setColour(juce::ToggleButton::textColourId, juce::Colour(250, 250, 250));
    
    // putting in major chord activator
    addAndMakeVisible (chordMinorButton);
    chordMinorButton.setButtonText("Play Minor Chords");
    chordMinorButton.setRadioGroupId(3);
    chordMinorButton.addListener(this);
	chordMinorButton.setColour(juce::ToggleButton::textColourId, juce::Colour(250, 250, 250));
	
    audioSourcePlayer.setSource (&synthAudioSource); // only change to add sound
    deviceManager.addAudioCallback (&audioSourcePlayer);

    setSize (800, 600);
}


void MainContentComponent::setButton(TextButton* button, String text) {
    button->setButtonText (text);
    button->addListener (this);
}


MainContentComponent::~MainContentComponent() {
    keyboardState.removeListener (this);
    audioSourcePlayer.setSource (nullptr);
    deviceManager.removeAudioCallback (&audioSourcePlayer);
}


void MainContentComponent::resized() {
    Rectangle<int> area (getLocalBounds());
    keyboardComponent.setBounds (area.removeFromTop (80).reduced(8));
    
    notesBox.setBounds (240, 275, 150, 225);
    rhythmBox.setBounds (410, 275, 150, 225);
    feedbackBox.setBounds(240, 525, 320, 50);
    
    recordButton.setBounds (620, 115, 150, 24);
    stopRecordButton.setBounds (620, 140, 150, 24);
    playNotesButton.setBounds (620, 165, 150, 24);
    combineButton.setBounds (620, 190, 150, 24);
    clearButton.setBounds (620, 215, 150, 24);
    
    notesButton.setBounds (420, 115, 150, 24);
    rhythmButton.setBounds (420, 140, 150, 24);

    sineButton.setBounds (50, 115, 150, 24);
    drumButton.setBounds (50, 140, 150, 24);
    bassButton.setBounds (50, 165, 150, 24);
    pianoButton.setBounds (50, 190, 150, 24);

    saveButton.setBounds (620, 375, 150, 24);
    loadButton.setBounds (620, 400, 150, 24);
    
    tutorialButton.setBounds(40, 551, 150, 24);
    
    singleNoteButton.setBounds(195, 115, 150, 24);
    chordMajorButton.setBounds (195, 140, 150, 24);
    chordMinorButton.setBounds(195, 165, 150, 24);
	
	feedbackLabel.setBounds(310, 245, 200, 20);
	instrumentsLabel.setBounds(40, 90, 200, 20);
	chordsLabel.setBounds(230, 90, 200, 20);
	editingLabel.setBounds(605, 90, 200, 20);
	melodyRhythm.setBounds(385, 90, 200, 20);
	MemoryLabel.setBounds(640, 350, 200, 20);

}


// This sets the appliations AudioDeviceManager, which handles the state of MIDI keyboard
AudioDeviceManager& MainContentComponent::getSharedAudioDeviceManager() {
    if (sharedAudioDeviceManager == nullptr) {
        sharedAudioDeviceManager = new AudioDeviceManager();
        RuntimePermissions::request (RuntimePermissions::recordAudio, runtimePermissionsCallback);
    }
    
    return *sharedAudioDeviceManager;
}




// need to implement this for application to run
void MainContentComponent::comboBoxChanged (ComboBox* box)
{
	// NEEDS TO BE IMPLEMENTED
}


void MainContentComponent::logNoteMessage (const String& m) {
    notesBox.moveCaretToEnd();
    notesBox.insertTextAtCaret (m + newLine);
}


void MainContentComponent::logRhythmMessage (const String& m) {
    rhythmBox.moveCaretToEnd();
    rhythmBox.insertTextAtCaret (m + newLine);
}


void MainContentComponent::logFeedback (const String& m) {
    feedbackBox.clear();
    feedbackBox.moveCaretToEnd();
    feedbackBox.insertTextAtCaret (m + newLine);
}


// Handle callbacks from the midi device and on-screen keyboard - Called every time we play a note
void MainContentComponent::handleIncomingMidiMessage (MidiInput* source, const MidiMessage& message) {
    const ScopedValueSetter<bool> scopedInputFlag (isAddingFromMidiInput, true);
    keyboardState.processNextMidiEvent (message);
}


void MainContentComponent::handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) {
    MidiMessage m (MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity));
    m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
    
    if (record and setNotes) {
        temp.push_back(m);

		NoteData temp;
		temp.note = MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3);
		temp.note_integer = m.getNoteNumber();
		temp.timeEnd = 0;
		temp.timeStart = 0;
		notes.push_back(temp);
		//notes.push_back(MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3));
        bufferNotes = notes;
        
        const MessageManagerLock mmLock;
        notesBox.clear();
        logNoteMessage("Notes: ");
        for (NoteData n : bufferNotes)
            logNoteMessage(n.note);
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
        bufferTimes = times;
        
        const MessageManagerLock mmLock;
        rhythmBox.clear();
        logRhythmMessage("Times: ");
        for (double n : bufferTimes)
            logRhythmMessage(String(n));
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
        bufferTimes = times;
        
        const MessageManagerLock mmLock;
        rhythmBox.clear();
        logRhythmMessage("Times: ");
        for (double n : bufferTimes)
            logRhythmMessage(String(n));
    }
}


MainContentComponent::IncomingMessageCallback::IncomingMessageCallback (MainContentComponent* o, const MidiMessage& m, const String& s)
: owner (o), message (m), source (s)
{}


void MainContentComponent::playNotes (std::vector<MidiMessage> temp) {
    if (!notes.empty()) {
        logFeedback("Playing notes.");
//        MidiInput*  MidiIn;
//        MidiIn = MidiInput::openDevice(0, this);
//    
//        for (int i = 0; i < temp.size(); i++) {
//            handleIncomingMidiMessage(MidiIn, temp[i]);
//            notes.erase(notes.begin());
//        }
    }
    else {
        logFeedback("Nothing to playback!");
    }
}


std::vector<NoteData> MainContentComponent::combineData(std::vector<NoteData> notes, std::vector<double> times) {
    std::vector<NoteData> bufferOut;
    
    if (notes.empty() || times.empty()) {
        logFeedback("Record (1) Notes and (2) Tempo before combining tracks.");
        return bufferOut; // Will be empty... TODO: handle this a better way
    }

    NoteData newNote;
    double timeZero = times[0]; // timeStart of first note, subtracted from all subsequent times
    
    for (int i = 0; i < notes.size(); ++i) {
        newNote.note = notes[i].note;
		newNote.note_integer = notes[i].note_integer;
        
        if (i < (times.size() / 2)) {
            newNote.timeStart = times[i * 2] - timeZero;
            newNote.timeEnd = times[(i * 2) + 1] - timeZero;
        }
        // Special case: user does not input rhythm for all notes
        // Default: set note to start 1.0 sec after last note ended, duration 0.5 seconds
        else {
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
    if (buttonThatWasClicked == &recordButton and record == false) {
        if (setNotes == false and setRhythm == false) {
            logFeedback("Select either \"Set notes\" or \"Set rhythm\" before recording.");
        }
        else {
            record = true;
        }
    }
    else if (buttonThatWasClicked == &stopRecordButton and record == true) {
        record = false;
    }
    else if (buttonThatWasClicked == &playNotesButton) {
        playNotes(temp);
    }
    else if (buttonThatWasClicked == &notesButton) {
        std::cout << "Clicked notes button." << std::endl;
        setNotes = true;
        setRhythm = false;
    }
    else if (buttonThatWasClicked == &rhythmButton) {
        std::cout << "Clicked rhythm button." << std::endl;
        setNotes = false;
        setRhythm = true;
    }
    else if (buttonThatWasClicked == &singleNoteButton) {
        chordValue = 0;
    }
    else if (buttonThatWasClicked == &chordMajorButton) {
        //        if (chordValue == 1)
        //            chordValue = 0;
        //        else
        chordValue = 1;
    }
    else if (buttonThatWasClicked == &chordMinorButton) {
        //        if (chordValue == 2)
        //            chordValue = 0;
        //        else
        chordValue = 2;
    }
    // Combine pitch and rhythm data
    else if (buttonThatWasClicked == &combineButton){
		bufferOut = combineData(bufferNotes, bufferTimes);
        if (!bufferOut.empty()) {
            String logstring = "";
            for (NoteData n : bufferOut) {
                logstring += n.note + " " + String(n.timeStart) + " " + String(n.timeEnd) + "\n";
            }
            logFeedback(logstring); 
        }
		is_combine_button = true;
    }
    else if (buttonThatWasClicked == &clearButton){
        bufferNotes.clear(); notes.clear();
        bufferTimes.clear(); times.clear();
        
        notesBox.clear();
        logNoteMessage("Notes: ");
        
        rhythmBox.clear();
        logRhythmMessage("Times: ");
        
        record = false;
        logFeedback("All recordings have been erased.");
    }
    else if (buttonThatWasClicked == &saveButton or buttonThatWasClicked == &loadButton) {
        const bool useNativeVersion = nativeButton.getToggleState();
        
        if (buttonThatWasClicked == &saveButton) {
            if (!notes.empty() and !bufferTimes.empty()) {
                FileChooser fc ("Choose a file to save...", File::getCurrentWorkingDirectory(), "*", useNativeVersion);
        
                if (fc.browseForFileToSave (true)) {
                    File chosenFile = fc.getResult();
                
                    std::ofstream file(chosenFile.getFullPathName().toStdString());
                    for (NoteData i : bufferNotes) {
                        file << i.note << std::endl;
                    }
                    file << "###" << std::endl;
                    for (double i : bufferTimes) {
                        file << String(i) << std::endl;
                    }
                    file.close();
                    logFeedback("File saved.");
                }
            }
            else {
                logFeedback("Need to record notes and times first.");
            }
        }
        else if (buttonThatWasClicked == &loadButton) {
            FileChooser fc ("Choose a file to open...", File::getCurrentWorkingDirectory(), "*", useNativeVersion);
            
            if (fc.browseForFileToOpen()) {
                String chosen;
                for (int i = 0; i < fc.getResults().size(); ++i)
                    chosen << fc.getResults().getReference(i).getFullPathName() << "\n";

                std::string chosenStr = chosen.toStdString();
                chosenStr.erase(std::remove(chosenStr.begin(), chosenStr.end(), '\n'), chosenStr.end());

                std::ifstream file(chosenStr);
                if (file.is_open()) {
                    bool time = false;
                    
                    std::string line;
                    while (std::getline(file, line)) {
                        if (line == "###") {
                            std::getline(file, line);
                            time = true;
                        }
                        if (!time) {
                            NoteData newNote;
                            newNote.note = String(line);
                            bufferNotes.push_back(newNote);
                            logNoteMessage(newNote.note);
                        }
                        else {
                            const char * newStr = line.c_str();
                            bufferTimes.push_back(std::atof(newStr));
                            logRhythmMessage(String(line));
                        }
                    }
                    file.close();
                    
                    logFeedback("File loaded.");
                }
            }
        }
    }
    else if (buttonThatWasClicked == &sineButton) {
        synthAudioSource.setUsingSineWaveSound();
    }
    else if (buttonThatWasClicked == &drumButton) {
        drum = true;
        bass = false;
        piano = false;
        synthAudioSource.setUsingSampledSound();
    }
    else if (buttonThatWasClicked == &bassButton) {
        drum = false;
        bass = true;
        piano = false;
        synthAudioSource.setUsingSampledSound();
    }
    else if (buttonThatWasClicked == &pianoButton) {
        drum = false;
        bass = false;
        piano = true;
        synthAudioSource.setUsingSampledSound();
    }
    else if (buttonThatWasClicked == &tutorialButton) {
        AlertWindow::showMessageBoxAsync (AlertWindow::InfoIcon,
                                          "Tutorial",
                                          "Thanks for using Piayes! We wish to make your piano-playing experience as enjoyable as possible. The general workflow is as follows:\n\t1. Select which instrument you want to use, and if you want to play a chord.\n\t2. Record a series of notes.\n\t3. Record the rhythm (you can use any key).\n\t4. Combine the notes and rhythms.\n\t5. Save the recording you just made.",
                                          "OK");
    }
}


void MainContentComponent::runtimePermissionsCallback (bool wasGranted) {
    int numInputChannels = wasGranted ? 2 : 0;
    sharedAudioDeviceManager->initialise (numInputChannels, 2, nullptr, true, String(), nullptr);
}


Component* createMainContentComponent()     { return new MainContentComponent(); }
