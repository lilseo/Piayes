####EECS 481 - Winter 2017 <br> ***Piayes* - Brad's DAW <br> Akansha Agarwal, Lillian Seo, Pedro Fialho, Yelena Pham <br> Alpha Release** <br> <br>

#####What is *Piayes*<br>
*Piayes* is an audio recording and editing application that provides an easy alternative for a user to create a song.<br>
Step 1: the user records a melody without worrying about the tempo/rhythm of the song.<br>
Step 2: the user records the desired tempo and length of each of the notes previously played by using any key on the keyboard.<br>
Step 3: the application will map the keys from step 1 to the timestamps from step 2 and output the resulting song.<br>
*Note that audio output will only be available on the Beta release.<br><br>

#####Please follow these steps to deploy *Piayes*<br>
1. Download the *JUCE* framework at https://www.juce.com/get-juce.<br>
2. Run *Piayes.jucer*, which will open up a *Projucer* window.<br>
3. Select *Config/Modules/juce_audio_basics*. There should be an error message regarding a path not found.<br>
4. Change the path to where *JUCE/modules* has been saved locally.<br>
5. Go back to *Config/Modules* and select all the path.<br>
6. Click on *Set paths for all modules* followed by *Copy the path from the module "juce_audio_basics" to all other modules*.<br>
7. Click on *Save Project and Open on IDE* on the lower left corner of the *Projucer* window.<br>
8. Build and run the application on your IDE.<br><br>

#####Using Piayes:<br>
1. Connect a MIDI keyboard to your machine or use the piano user interface.<br>
2. Click on the "Record" button and select the "Set Notes" mode to save the keys of the notes played (opposite order works too).<br>
3. The application will save and output the keys, but not the timestamps, of all the notes played.<br>
4. Click on "Playback Notes" to output the keys of all the notes recorded so far.<br>
5. Select "Set Rhythm" mode to save the key-down and key-up timestamps of all the notes played.<br>
6. The application will save and output the timestamps, but not the keys, of all the notes played.<br> 
7. Click on "Stop Recording" and then "Combine Tracks" to output the keys mapped to the desired rhythm.<br>
8. Note that the same number of notes should be played on the two modes for the application to work correctly.<br>
9. You may stop recording and delete everything at any point by clicking on "Clear Recording".<br>
