PiaNO ... more like PiaYES. Welcome to **Brad's Really Accessible Daw (B.R.A.D.) : Piayes Edition**, a digital audio workstation customized to fit the needs of our main user, Brad, and help him play piano again. <br>

##### EECS 481 - Winter 2017 <br>
Brad's DAW Group 1 - *Piayes* <br> Akansha Agarwal, Lillian Seo, Pedro Fialho, Yelena Pham <br>
Brad's DAW GROUP 2 - *B.R.A.D.* <br> Anna Billings, Mathew Wiesman, Chris Salem, Dan Verderese <br> <br>

### What is **_B.R.A.D.:Piayes Edition_** <br>
**B.R.A.D.:Piayes Edition** is an audio recording and playing application that provides an easy alternative for a user to create a song. <br>
Step 1: the user records a melody without worrying about the tempo/rhythm of the song. <br>
Step 2: The user can then play a rhythm and rather than have to worry about the notes they are playing, the application will play the notes that were recorded from the melody no matter the key played. <br>
Additionally, melodies can be saved, loaded, and combined together so the user can always return to a melody they are working on. <br>

### Features <br>
Chord Triggering <br>
Octave Switching at the touch of a button <br>
Record Melody/Playalong <br>
Save/Load/Combine Melodies <br>
Change Instruments <br>
And Many More <br>

### UI (Omega) <br>
![alt tag](https://cloud.githubusercontent.com/assets/14114194/25111329/4e2aa630-23b8-11e7-8e8b-16d8df784eba.png) 
<br>
When designing the UI we wanted what the user would be looking at to look as similar as possible to the mapped controller (picture below) in order to make the experience and usability as seamless and intuitive as possible. The UI, then, effectively acts as visual aid. <br>
![alt tag](https://cloud.githubusercontent.com/assets/14114194/25111236/afc77b80-23b7-11e7-9a76-bc9135449d04.png)
<br>

#### Deploying **_B.R.A.D.:Piayes Edition_** <br>
Plug the MIDI keyboard in and run the provided executable. <br>

#### Using **_B.R.A.D.:Piayes Edition_** <br>
1. Connect a MIDI keyboard to your machine or use the piano user interface.<br>
2. Click on the **Record** button and select the **Set Notes** mode to save the keys of the notes played.<br>
3. The application will save and output the keys, but not the timestamps, of all the notes played.<br>
4. Click on **Playback Notes** to output the keys of all the notes recorded so far.<br>
5. Select **Set Rhythm** mode to save the key-down and key-up timestamps of all the notes played.<br>
6. The application will save and output the timestamps, but not the keys, of all the notes played.<br> 
7. Click on **Stop Recording** and then **Combine Tracks** to output the keys mapped to the desired rhythm.<br>
8. Note that the same number of notes should be played on the two modes for the application to work correctly.<br>
9. You may stop recording and delete everything at any point by clicking on **Clear Recording**.<br>


#### Combining Team **_Piayes_** with Team **_B.R.A.D._** (Beta) <br>
In the Beta release we have aimed to combine our groups together in order to give Brad a better playing experience. After working and learning our systems for the alpha release we met up and made the decision that it was best for Team B.R.A.D. to take their ideas and their code and integrate it into Team Piayes' project (and git hub repo) since going the other way would have been substantially more difficult and would only result in a slightly better design. We felt that in the end our product should be focused on creating something to help Brad play again and not on splitting things in such a way that each groups contributions are easily identifiable all at the cost of making it harder to use. <br> 
While Team Piayes will continuing to focus on the editing features outlined above, Team B.R.A.D. will focus on the "live" playing aspects, including volume control, octave switching, and chord triggering. <br>
For more information on Team B.R.A.D (made up of Mathew Wiesman, Anna Billings, Daniel Verderese, and Chris Salem) see the original github repo: https://github.com/mwiesman/B.R.A.D.<br>

#### Working with **_B.R.A.D.:Piayes Edition_** On Your Own <br>
1. Download JUCE from https://www.juce.com/get-juce <br> 
2. Download/Clone the Piayes repo from https://github.com/lilseo/Piayes <br>
	1. Press the green “Clone or download” button > press “Download ZIP” <br> 
3. Open the Piayes.Jucer file from the Piayes repo directory in the ProJucer application<br> 
4. From the Config tab:<br> 
	1. Select “juce_audio_basics” under “Modules”<br> 
	2. Change the “Path for ‘Xcode (MacOSX)’” to be the the Modules folder in your copy of JUCE<br> 
	3. Click on “Modules” in the left-side menu <br> 
	4. Click on “juce_audio_basics” > “Set path for all modules” button > “Copy the path from the module ‘juce_audio_basics’ to all other modules<br> 
	5. Select “Piayes” (or the name of the project) and make sure “include BinaryData.h in the AppConfig.h file” is UNCHECKED<br> 
5. From the files tab:<br> 
	1. Make sure the “Source” folder contains BinaryData.cpp, BinaryData.h, MainComponent.h, MainComponent.cpp, and Main.Cpp. Only the cpp files should have “compile” checked. <br> 
6. Click “Save Project and Open in IDE…” to open the project in XCode/the IDE selected<br> 
7. Within XCode:<br> 
	1. Add the “Synth.cpp” and “Synth.h” into the Source file in the project. <br> 
			* Ctrl-click on the “Source” folder in the left-side menu in XCode. <br>
			* Select “Add files to Piayes”. <br>
			* The “Synth.cpp” and “Synth.h” files are located in the source folder in the main project directory that was cloned from Github. <br>
	2. Under the “Juce Modules/juce_audio_basics/midi” folder there are two files:<br> 
	“juce_MidiKeyboardState.cpp” and “juce_MidiKeyboardState.h”<br> 
	3. In the Piayes project folder there is a folder called “juceCopies” that contains a modified version above two files. Copy the contents of these files into the ones in the midi folder and uncomment them. <br> 
	4. In the left-side menu in XCode under “Source”, there are three .wav files: “bass.wav”, “Electro-Tom.wav”, and “piano.wav” <br>
			* For each of the three .wav files: Click on the file > Locate the “Target Membership” box in the right-side menu in XCode > Check the box under Target Membership, labeled “Piayes(App)” <br>
	5. Build (this can take up to several minutes), Compile and Run!<br> 
8. Play!<br> 

Make an executable: Xcode -> Product -> Archive <br>
Click the Export button -> Export as a Mac Application -> Save it <br> 

Open and Play!<br> 
