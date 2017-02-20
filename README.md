EECS 481 - Winter 2017
Piayes - Brad's DAW 
Akansha Agarwal, Lillian Seo, Pedro Fialho, Yelena Pham
Alpha Release

What is Piayes:
Piayes is an audio recording and editing application that provides an easy alternative for a user to create a song.
Step 1: the user records a melody without worrying about the tempo/rhythm of the song.
Step 2: the user records the desired tempo and length of each of the notes previously played by using any key on the keyboard.
Step 3: the application will map the keys from step 1 to the timestamps from step 2 and output the resulting song.
*Note that audio output will only be available on the Beta release.

Please follow these steps to deploy Piayes:
1. Download the JUCE framework at https://www.juce.com/get-juce.
2. Run Piayes.jucer, which will open up a Projucer window.
3. Select Config/Modules/juce_audio_basics. There should be an error message regarding a path not found.
4. Change the path to where JUCE/modules has been saved locally.
5. Go back to Config/Modules and select all the path. 
6. Click on "Set paths for all modules" followed by "Copy the path from the module "juce_audio_basics" to all other modules".
7. Click on "Save Project and Open on IDE" on the lower left corner of the Projucer window.
8. Build and run the application on your IDE.

Using Piayes:
1. Connect a MIDI keyboard to your machine or use the piano user interface.
2. Click on the "Record" button and select the "Set Notes" mode to save the keys of the notes played (opposite order works too).
3. The application will save and output the keys, but not the timestamps, of all the notes played. 
4. Click on "Playback Notes" to output the keys of all the notes recorded so far.
5. Select "Set Rhythm" mode to save the key-down and key-up timestamps of all the notes played.
6. The application will save and output the timestamps, but not the keys, of all the notes played. 
7. Click on "Stop Recording" and then "Combine Tracks" to output the keys mapped to the desired rhythm.
8. Note that the same number of notes should be played on the two modes for the application to work correctly.
9. You may stop recording and delete everything at any point by clicking on "Clear Recording".
