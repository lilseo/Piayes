#include "MainComponent.h"


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

void SineWaveVoice::startNote (int midiNoteNumber, float velocity,
                    SynthesiserSound*, int /*currentPitchWheelPosition*/) {
    currentAngle = 0.0;
    level = velocity * 0.15;
    tailOff = 0.0;

    double cyclesPerSecond = MidiMessage::getMidiNoteInHertz (midiNoteNumber);
    double cyclesPerSample = cyclesPerSecond / getSampleRate();

    angleDelta = cyclesPerSample * 2.0 * double_Pi;
}

void SineWaveVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        // start a tail-off by setting this flag. The render callback will pick up on
        // this and do a fade out, calling clearCurrentNote() when it's finished.

        if (tailOff == 0.0) // we only need to begin a tail-off if it's not already doing so - the
                            // stopNote method could be called more than once.
            tailOff = 1.0;
    }
    else
    {
        // we're being told to stop playing immediately, so reset everything..

        clearCurrentNote();
        angleDelta = 0.0;
    }
}

void SineWaveVoice::pitchWheelMoved (int /*newValue*/) {}

void SineWaveVoice::controllerMoved (int /*controllerNumber*/, int /*newValue*/) {}

void SineWaveVoice::renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples) {
    int count = 0;
    if (angleDelta != 0.0)
    {
        if (tailOff > 0)
        {
            while (--numSamples >= 0)
            {
                const float currentSample = (float) (std::sin (currentAngle) * level * tailOff);
                
                for (int i = outputBuffer.getNumChannels(); --i >= 0;) {
                    outputBuffer.addSample (i, startSample, currentSample);
                }

                currentAngle += angleDelta;
                ++startSample;

                tailOff *= 0.99;
                count++;
                if (tailOff <= 0.005)
                {
                    clearCurrentNote();
                    
                    angleDelta = 0.0;
                    break;
                }
            }
            std::cout << count << std::endl;

        }
        else
        {
            while (--numSamples >= 0)
            {
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
    for (int i = 4; --i >= 0;)
    {
        synth.addVoice (new SineWaveVoice());   // These voices will play our custom sine-wave sounds..
        synth.addVoice (new SamplerVoice());    // and these ones play the sampled sounds
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

    ScopedPointer<AudioFormatReader> audioReader (wavFormat.createReaderFor (new MemoryInputStream (BinaryData::cello_wav,
                                                                                                    BinaryData::cello_wavSize,
                                                                                                    false),
                                                                                true));

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

void SynthAudioSource::prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) {
    midiCollector.reset (sampleRate);

    synth.setCurrentPlaybackSampleRate (sampleRate);
}

void SynthAudioSource::releaseResources() {}

void SynthAudioSource::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) {
    // the synth always adds its output to the audio buffer, so we have to clear it
    // first..
    bufferToFill.clearActiveBufferRegion();

    // fill a midi buffer with incoming messages from the midi input.
    MidiBuffer incomingMidi;
    midiCollector.removeNextBlockOfMessages (incomingMidi, bufferToFill.numSamples);

    // pass these messages to the keyboard state so that it can update the component
    // to show on-screen which keys are being pressed on the physical midi keyboard.
    // This call will also add midi messages to the buffer which were generated by
    // the mouse-clicking on the on-screen keyboard.
    // if the chord feature is active run the overloaded version pertaining to that
    if (chordValue)
        keyboardState.processNextMidiBuffer (incomingMidi, 0, bufferToFill.numSamples, true, chordValue);
    else
        keyboardState.processNextMidiBuffer (incomingMidi, 0, bufferToFill.numSamples, true);


    // and now get the synth to process the midi events and generate its output.
    synth.renderNextBlock (*bufferToFill.buffer, incomingMidi, 0, bufferToFill.numSamples);
}



MainContentComponent::MainContentComponent()
: deviceManager (getSharedAudioDeviceManager()),
lastInputIndex (0),
isAddingFromMidiInput (false),
synthAudioSource (keyboardState),
keyboardComponent (keyboardState, MidiKeyboardComponent::horizontalKeyboard),
startTime (Time::getMillisecondCounterHiRes() * 0.001) {
    
    
    // Set up MIDI input if keyboard is plugged in
    const StringArray midiInputs (MidiInput::getDevices());
    midiInputList.addItemList (midiInputs, 1);
    midiInputList.addListener (this);
    
    // Checks if there are any enabled devices - never true for alpha, might be useful for beta
    for (int i = 0; i < midiInputs.size(); ++i)
    {
        if (deviceManager.isMidiInputEnabled(midiInputs[i]))
        {
            setMidiInput (i);
            break;
        }
    
    }
    
    // If no MIDI enabled - always true for alpha, might be useful for beta
    if (midiInputList.getSelectedId() == 0) {
        setMidiInput (0); // called with or without MIDI keyboard
    }
    
    // MIDI Outputs - not working yet
    midiOutputList.addItemList (MidiOutput::getDevices(), 1);
    midiOutputList.addListener (this);
    
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
    
    // putting in major chord activator
    addAndMakeVisible (chordMajorButton);
    chordMajorButton.setButtonText("Play Major Chords");
    chordMajorButton.addListener(this);
    
    
    audioSourcePlayer.setSource (&synthAudioSource);
    deviceManager.addAudioCallback (&audioSourcePlayer);

    
    setSize (600, 400);
}

MainContentComponent::~MainContentComponent() {
    keyboardState.removeListener (this);
    audioSourcePlayer.setSource (nullptr);
    deviceManager.removeAudioCallback (&audioSourcePlayer);
}

void MainContentComponent::resized() {
    //(Re)sizing of GUI buttons
    
    Rectangle<int> area (getLocalBounds());
    keyboardComponent.setBounds (area.removeFromTop (80).reduced(8));
    
    notesBox.setBounds (200, 100, 150, 225);
    rhythmBox.setBounds (400, 100, 150, 225);
    feedbackBox.setBounds(200, 337.5, 350, 50);
    
    recordButton.setBounds (16, 100, 150, 24);
    stopRecordButton.setBounds (16, 125, 150, 24);
    playNotesButton.setBounds (16, 150, 150, 24);
    combineButton.setBounds (16, 175, 150, 24);
    clearButton.setBounds (16, 200, 150, 24);
    notesButton.setBounds (16, 225, 150, 24);
    rhythmButton.setBounds (16, 250, 150, 24);
    
    chordMajorButton.setBounds (16, 275, 150, 24);
    
}


// This sets the appliations AudioDeviceManager, which handles the state of MIDI keyboard
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
    const StringArray list (MidiInput::getDevices()); // list will contain only the MIDI keyboard, if connected
    
    deviceManager.removeMidiInputCallback (list[lastInputIndex], this);
    
    const String newInput(list[index]); // newInput is either the MIDI keyboard or null if we're just using the UI
    
    if (! deviceManager.isMidiInputEnabled(newInput)) { // always the case for alpha
        deviceManager.setMidiInputEnabled (newInput, true);
    }
    
    // MididInputCallback receives messages from a physical MIDI input device
    deviceManager.addMidiInputCallback (newInput, this);
    midiInputList.setSelectedId (index + 1, dontSendNotification);
    
    lastInputIndex = index;
}

// Not working yet - beta material
void MainContentComponent::setMidiOutput (int index)
{
    currentMidiOutput = nullptr;
    
    if (MidiOutput::getDevices() [index].isNotEmpty())
    {
        currentMidiOutput = MidiOutput::openDevice (index);
        jassert (currentMidiOutput);
    }
}


// need to implement this for application to run
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
    
//    if (chordValue) {
////        std::cout << m.getNoteNumber() << std::endl;
////        std::cout << m.getDescription() << std::endl;
//        MidiMessage m3(MidiMessage::noteOn (midiChannel, m.getNoteNumber() + 4, velocity)); // the major third
//        //m3.setNoteNumber(m.getNoteNumber() + 4);
//        MidiMessage m5(m); // the perfect 5th
//        m5.setNoteNumber(m.getNoteNumber() + 7);
//        std::cout << m.getNoteNumber() << ' ' << m3.getNoteNumber() << ' ' << m5.getNoteNumber() << std::endl;
//        std::cout << midiChannel << std::endl;
//    }
    
    if (record and setNotes) {
        std::cout << MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3) << std::endl;
        notes.push_back(MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3));
        bufferNotes = notes;
        std::cout << "Notes: ";
        for (String n : bufferNotes)
            std::cout << n << ' ';
        std::cout << std::endl;
        
        const MessageManagerLock mmLock;
        notesBox.clear();
        logNoteMessage("Notes: ");
        for (String n : bufferNotes)
            logNoteMessage(n);
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
        std::cout << timecode << std::endl;
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

void MainContentComponent::playNotes () {
    String logstring = "";
    for (int i = 0; i < bufferNotes.size(); i++) {
        int midiNote =  convertNameToMidi(bufferNotes[i]);
        std::cout << bufferNotes[i] << " : " << midiNote << std::endl;
        logstring += bufferNotes[i] + " : " + String(midiNote) + "\n";
        notesMidi.push_back(midiNote);
    }
    logFeedback(logstring);
}

std::vector<NoteData> MainContentComponent::combineData(std::vector<String> notes, std::vector<double> times){
    std::vector<NoteData> bufferOut;
    std::cout << "size of notes: "<< notes.size() << std::endl << "size of times:" << times.size() << std::endl;
    if (notes.empty() || times.empty()){
        std::cout << "Record (1) Notes and (2) Tempo before combining tracks!" << std::endl;
        logFeedback("Record (1) Notes and (2) Tempo before combining tracks!");
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
        if (setNotes == false and setRhythm == false) {
            std::cout << "Select either \"Set notes\" or \"Set rhythm\" before recording." << std::endl;
            logFeedback("Select either \"Set notes\" or \"Set rhythm\" before recording.");
        }
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
        setNotes = true;
        setRhythm = false;
    }
    else if (buttonThatWasClicked == &rhythmButton) {
        setNotes = false;
        setRhythm = true;
    }
    else if (buttonThatWasClicked == &chordMajorButton) {
        chordValue = 1;
    }
    // Combine pitch and rhythm data
    else if (buttonThatWasClicked == &combineButton){
        bufferOut = combineData(bufferNotes, bufferTimes);
        if (!bufferOut.empty()) {
            String logstring = "";
            std::cout << "bufferOut: " << std::endl;
            for (NoteData n : bufferOut) {
                std::cout << n.note << ' ' << n.timeStart << ' ' << n.timeEnd << std::endl;
                logstring += n.note + " " + String(n.timeStart) + " " + String(n.timeEnd) + "\n";
            }
            logFeedback(logstring); 
        }
    }
    else if (buttonThatWasClicked == &clearButton){
        bufferNotes.clear();
        notes.clear();
        bufferTimes.clear();
        times.clear();
        notesBox.clear();
        logNoteMessage("Notes: ");
        rhythmBox.clear();
        logRhythmMessage("Times: ");
        record = false;
        std::cout << "All recordings have been erased." << std::endl;
        logFeedback("All recordings have been erased.");
    }
}

void MainContentComponent::runtimePermissionsCallback (bool wasGranted) {
    int numInputChannels = wasGranted ? 2 : 0;
    sharedAudioDeviceManager->initialise (numInputChannels, 2, nullptr, true, String(), nullptr);
}

Component* createMainContentComponent()     { return new MainContentComponent(); }
